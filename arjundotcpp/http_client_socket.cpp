#include "http_client_socket.hpp"

bool
http_client_socket_t::read_line(sock_t* sock, std::string& dst)
{
	bool has_cr(false), has_nl(false);

	if (nullptr == sock)
		return false;

	do {
		std::string data("");

		if (false == sock->read(data, 1))
			return false;

		if ('\r' == data[0]) {
			if (true == has_cr)
				return false; // malformed

			has_cr = true;
		}
		else if ('\n' == data[0]) {
			if (true == has_nl)
				return false; // malformed

			has_nl = true;
		}
		else
			dst += data[0];
	} while (false == has_cr || false == has_nl);

	return true;
}

bool
http_client_socket_t::recv_status_line(sock_t* sock, http_response_t& resp)
{
	bool		has_cr(false), has_nl(false);
	std::string scratch(""), value("");

	if (nullptr == sock)
		return false;

	if (false == read_line(sock, scratch))
		return false;

	if (scratch.compare(0, 5, "HTTP/")) // malformed
		return false;

	scratch = scratch.substr(5);

	if (!scratch.compare(0, 3, "0.9"))
		resp.version(http_version_t::HTTP_VERSION_09);
	else if (!scratch.compare(0, 3, "1.0"))
		resp.version(http_version_t::HTTP_VERSION_10);
	else if (!scratch.compare(0, 3, "1.1"))
		resp.version(http_version_t::HTTP_VERSION_11);
	else
		return false; // unsupported version

	scratch = scratch.substr(3);

	scratch = trim_whitespace(scratch);

	for (std::size_t idx = 0; idx < scratch.length(); idx++)
		if (' ' == scratch[idx])
			break;
		else
			value += scratch[idx];

	resp.status(std::strtoul(value.c_str(), nullptr, 10));

	scratch = scratch.substr(value.length());
	scratch = trim_whitespace(scratch);
	resp.status_text(scratch);

	return true;
}

bool
http_client_socket_t::recv_headers(sock_t* sock, http_response_t& resp)
{
	std::string line("");
	std::string name(""), value("");

	if (nullptr == sock)
		return false;

	do {
		if (false == read_line(sock, line))
			return false;
		if (!line.length()) // line started with \r\n
			break;

		if (std::string::npos == line.find_first_of(':'))
			return false; // malformed header line

		name = trim_whitespace(line.substr(0, line.find_first_of(':')));
		value = trim_whitespace(line.substr(line.find_first_of(':') + 1));

		resp.add_header(name, value);
		line.clear();
	} while (true);

	return true;
}

bool 
http_client_socket_t::recv_body(sock_t* sock, http_response_t& resp)
{
	if (nullptr == sock)
		return false;

	if (true == resp.has_header("Content-Length")) {
		std::size_t length(std::strtoul(resp.get_header("Content-Length").c_str(), nullptr, 10));
		std::string body;

		if (false == sock->read(body, length))
			return false;

		resp.body(body);
		return true;
	}

	if (true == resp.has_header("Transfer-Encoding")) {
		if (std::string::npos != resp.get_header("Transfer-Encoding").find_first_of("hunked")) {
			// oh that HTTP data stream...he's so dreamy...
			std::size_t chunk_len(0);
			std::string line("");
			std::string body("");

			do {
				if (false == read_line(sock, line))
					return false;
				if (!line.length())
					continue;
				chunk_len = std::strtoul(line.c_str(), nullptr, 16);

				if (false == sock->read(body, chunk_len))
					return false;

				resp.append_body(body);
				body.clear();
				line.clear();
			} while (0 != chunk_len);

			return true;
		}
	}

	return true;
}

bool 
http_client_socket_t::recv_resp(sock_t* sock, http_response_t& resp)
{
	if (false == recv_status_line(sock, resp))
		return false;
	if (false == recv_headers(sock, resp))
		return false;
	if (false == recv_body(sock, resp))
		return false;

	return true;
}

bool
http_client_socket_t::get(const std::string url, const http_parameters_t& data, const header_map_t& headers, http_response_t& resp, bool verify)
{
	std::string			transport_type("");
	std::string			domain("");
	std::string			port("");
	std::string			uri("");
	http_get_request_t	req;
	sock_t*				sock(nullptr);

	if (true == verify)
		return false; // SSL verification not implemented yet

	if (false == split_url(url, transport_type, domain, port, uri))
		return false;

	if (! transport_type.compare(0, 5, "https"))
		sock = new ssl_sock_t;
	else
		sock = new sock_t;

	req.uri(uri);

	if (false == sock->connect(domain, port))
		return false;

	for (std::size_t idx = 0; idx < data.size(); idx++)
		req.add_parameter(data[idx]);
	for (header_map_t::const_iterator itr = headers.begin(); itr != headers.end(); itr++)
		req.add_header(itr->first, itr->second);

	if (req.version() == http_version_t::HTTP_VERSION_11)
		req.add_header("Host", domain);

	if (false == sock->write(req.to_string()))
		return false;

	if (false == recv_resp(sock, resp))
		return false;

	return true;
}

bool 
http_client_socket_t::post(const std::string url, const http_parameters_t& data, const header_map_t& headers, http_response_t& resp, http_content_type_t content_type, bool verify)
{
	std::string			transport_type("");
	std::string			domain("");
	std::string			port("");
	std::string			uri("");
	http_post_request_t req;
	sock_t*				sock(nullptr);

	if (true == verify)
		return false; // SSL verification not implemented yet

	if (false == split_url(url, transport_type, domain, port, uri))
		return false;

	if (! transport_type.compare(0, 5, "https"))
		sock = new ssl_sock_t;
	else
		sock = new sock_t;

	req.uri(uri);

	if (false == sock->connect(domain, port))
		return false;

	for (std::size_t idx = 0; idx < data.size(); idx++)
		req.add_parameter(data[idx]);
	for (header_map_t::const_iterator itr = headers.begin(); itr != headers.end(); itr++)
		req.add_header(itr->first, itr->second);

	req.content_type(content_type);

	if (req.version() == http_version_t::HTTP_VERSION_11)
		req.add_header("Host", domain);

	if (false == sock->write(req.to_string()))
		return false;

	if (false == recv_resp(sock, resp))
		return false;

	return true;
}

bool 
http_client_socket_t::head(const std::string url, const header_map_t& headers, http_response_t& resp, bool verify)
{
	std::string			transport_type("");
	std::string			domain("");
	std::string			port("");
	std::string			uri("");
	http_head_request_t req;
	sock_t*				sock(nullptr);

	if (true == verify)
		return false; // SSL verification not implemented yet

	if (false == split_url(url, transport_type, domain, port, uri))
		return false;

	if (!transport_type.compare(0, 5, "https"))
		sock = new ssl_sock_t;
	else
		sock = new sock_t;

	req.uri(uri);

	if (false == sock->connect(domain, port))
		return false;

	for (header_map_t::const_iterator itr = headers.begin(); itr != headers.end(); itr++)
		req.add_header(itr->first, itr->second);

	if (req.version() == http_version_t::HTTP_VERSION_11)
		req.add_header("Host", domain);

	if (false == sock->write(req.to_string()))
		return false;

	if (false == recv_resp(sock, resp))
		return false;

	return true;
}

bool 
http_client_socket_t::split_url(const std::string& url, std::string& transport, std::string& domain, std::string& port, std::string& uri)
{
	std::regex	url_regex("^(https?)://([a-zA-Z0-9-_\\.]+):?([0-9]+)?(/.*)?$");
	std::smatch url_matches;

	if (std::regex_match(url, url_matches, url_regex)) {
		if (false == url_matches[1].matched)
			return false;
		else
			transport = url_matches[1].str();

		if (false == url_matches[2].matched)
			return false;
		else
			domain = url_matches[2].str();

		if (false == url_matches[3].matched) {
			if (! transport.compare(0, 5, "https"))
				port = "443";
			else if (! transport.compare(0, 4, "http"))
				port = "80";
			else
				return false;
		}
		else
			port = url_matches[3].str();

		if (false == url_matches[4].matched)
			uri = "/";
		else
			uri = url_matches[4].str();
	}

	return true;
}