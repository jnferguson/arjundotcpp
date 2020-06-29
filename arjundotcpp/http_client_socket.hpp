#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <regex>
#include "http_common.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "sock.hpp"
#include "ssl_sock.hpp"
#include "util.hpp"


class http_client_socket_t
{
	private:
	protected:
		static bool read_line(sock_t* sock, std::string& dst);
		static bool recv_status_line(sock_t* sock, http_response_t& resp);
		static bool recv_headers(sock_t* sock, http_response_t& resp);
		static bool recv_body(sock_t* sock, http_response_t& resp);

		static bool recv_resp(sock_t* sock, http_response_t& resp);

	public:
		http_client_socket_t(void) = default;
		~http_client_socket_t(void) = default;

		static bool split_url(const std::string& url, std::string& transport, std::string& domain, std::string& port, std::string& uri);
		static bool get(const std::string url, const http_parameters_t& data, const header_map_t& headers, http_response_t& resp, bool verify = false);
		static bool post(const std::string url, const http_parameters_t& data, const header_map_t& headers, http_response_t& resp, http_content_type_t content_type = http_content_type_t::CONTENT_TYPE_MULTIPART_FORM_DATA, bool verify = false);
		static bool head(const std::string url, const header_map_t& headers, http_response_t& resp, bool verify = false);
};

