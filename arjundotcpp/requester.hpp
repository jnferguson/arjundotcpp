#pragma once
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <regex>
#include <iostream>
#include "config.hpp"
#include "http_client_socket.hpp"
#include "http_common.hpp"
#include "http_response.hpp"
#include "rand.hpp"

class requester_t
{
	private:
	protected:
	public:
		requester_t(void) = default;
		~requester_t(void) = default;
		
		static http_response_t* request(const std::string& url, const http_parameters_t& params, const header_map_t& headers, const bool is_get, const std::size_t delay);
		static bool stabilize(std::string& url, header_map_t& headers);
};

