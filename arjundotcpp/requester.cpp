#include "requester.hpp"

http_response_t*
requester_t::request(const std::string& url, const http_parameters_t& params, const header_map_t& headers, const bool is_get, const std::size_t delay)
{
	http_response_t* ret(new http_response_t);
	std::size_t dlay(delay);

	if (true == config_t::instance().prefer_stability()) {
		rand_t rnd;

		do {
			dlay = rnd.byte() % 12;
		} while (6 > delay && 12 < delay);
	}

	std::this_thread::sleep_for(std::chrono::seconds(dlay));

	if (true == is_get) {
		if (false == http_client_socket_t::get(url, params, headers, *ret, false)) {
			delete ret;
			return nullptr;
		}
	} else if (true == config_t::instance().json()) {
		if (false == http_client_socket_t::post(url, params, headers, *ret, http_content_type_t::CONTENT_TYPE_JSON, false)) {
			delete ret;
			return nullptr;
		}
	} else {
		if (false == http_client_socket_t::post(url, params, headers, *ret, http_content_type_t::CONTENT_TYPE_MULTIPART_FORM_DATA, false)) {
			delete ret;
			return nullptr;
		}
	}

	return ret;
}


bool 
requester_t::stabilize(std::string& url, header_map_t& headers)
{
	std::regex	url_regex("^(https?)://(.*)$");
	std::smatch url_matches;
	std::string ret("");
	http_response_t resp;

	if (std::regex_match(url, url_matches, url_regex)) {
		if (false == url_matches[1].matched) {
			ret = "http://" + url;
			if (false == http_client_socket_t::head(ret, headers, resp, false)) {
				ret = "https://" + url;

				if (false == http_client_socket_t::head(ret, headers, resp, false)) {
					return false;
				}
				else {
					url = ret;
					return true;
				}
			} else {
				url = ret;
				return true;
			}
		}
	}

	return true;
}