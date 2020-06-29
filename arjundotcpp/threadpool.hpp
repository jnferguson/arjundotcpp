#pragma once
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <deque>
#include <vector>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <functional>		
#include "http_common.hpp"
#include "http_response.hpp"
#include "http_client_socket.hpp"
#include "config.hpp"
#include "requester.hpp"
#include "globals.hpp"

typedef std::deque< stringvector_t > wordlist_queue_t;

typedef std::function< bool(http_response_t*, const http_parameters_t&, const http_parameters_t&) > process_response_function_ptr_t;

class threadpool_t
{
	private:
	protected:
		std::mutex					m_mutex;
		wordlist_queue_t			m_queue;
		std::size_t					m_nthreads;
		std::vector< std::thread* > m_threads;
		std::atomic_bool			m_run;
		std::atomic_bool			m_done;
		const std::string			m_url;
		const http_parameters_t		m_params;
		const header_map_t			m_headers;
		process_response_function_ptr_t m_process_response;

		void push_back(stringvector_t& val);
		stringvector_t pop_front(void);
		bool is_empty(void);

		void do_work(void);

		void split_wordlist(stringvector_t& wl, std::size_t split_cnt = 2);

	public:
		threadpool_t(const std::string& url, http_parameters_t& params, header_map_t& headers, stringvector_t& wordlist, std::size_t split_cnt = 50, std::size_t nthreads = 2);

		void set_process_response(std::function< bool(http_response_t*, const http_parameters_t&, const http_parameters_t&) >&);
		void start(void);
		void stop(void);
		bool finished(void);
};

