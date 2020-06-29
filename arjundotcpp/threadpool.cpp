#include "threadpool.hpp"

void
threadpool_t::push_back(stringvector_t& val)
{
	std::lock_guard< std::mutex > lck(m_mutex);
	m_queue.push_back(val);
	return;
}

stringvector_t
threadpool_t::pop_front(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);
	stringvector_t ret;

	if (m_queue.empty())
		throw std::runtime_error("threadpool_t::pop_front(): attempted to pop_front() of empty work queue");

	ret = m_queue.front();
	m_queue.pop_front();

	return ret;
}

bool
threadpool_t::is_empty(void)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (m_queue.empty())
		return true;

	return false;
}

void
threadpool_t::do_work(void)
{
	while (true == m_run) {
		stringvector_t wl;
		http_parameters_t params;
		http_response_t* resp(nullptr);

		while (is_empty()) {
			m_done = true;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		wl = pop_front();

		for (std::size_t idx = 0; idx < m_params.size(); idx++)
			params.push_back(m_params[idx]);

		for (std::size_t idx = 0; idx < wl.size(); idx++) {
			string_pair_request_data_t* param(new string_pair_request_data_t);
			param->parameter_name(wl.at(idx));
			param->parameter_value(random_decstring(6));
			params.push_back(param);
		}


		resp = requester_t::request(m_url, params, m_headers, config_t::instance().get(), config_t::instance().delay());

		if (nullptr == resp) {
			push_back(wl);
			continue;
		}

		if (nullptr != m_process_response) {
			if (true == m_process_response(resp, params, m_params)) {
				if (1 == wl.size()) {
					globals_t::instance().add_found_parameter(wl[0]);
					continue;
				} else
					split_wordlist(wl, 2);
			}
		}
		delete resp;
	}

	return;
}

void
threadpool_t::split_wordlist(stringvector_t& wl, std::size_t split_cnt)
{
	std::size_t div(wl.size() / split_cnt);
	std::size_t mod(wl.size() % split_cnt);

	for (std::size_t idx = 0; idx < split_cnt; idx++) {
		stringvector_t tmp;
		std::size_t start_idx(idx * div + std::min(idx, mod));
		std::size_t end_idx((idx + 1) * div + std::min(idx + 1, mod));

		for (std::size_t inner_idx = start_idx; inner_idx < end_idx; inner_idx++)
			tmp.push_back(wl.at(inner_idx));

		m_queue.push_back(tmp);
		m_done = false;
	}

	return;
}

threadpool_t::threadpool_t(const std::string& url, http_parameters_t& params, header_map_t& headers, stringvector_t& wordlist, std::size_t split_cnt, std::size_t nthreads)
	: m_nthreads(nthreads), m_url(url), m_params(params), m_headers(headers), m_run(false), m_process_response(nullptr)
{
	split_wordlist(wordlist, split_cnt);
}

void
threadpool_t::set_process_response(std::function< bool(http_response_t*, const http_parameters_t&, const http_parameters_t&) >& fptr)
{
	m_process_response = fptr;
	return;
}

void
threadpool_t::start(void)
{
	if (true == m_run)
		return;

	if (0 != m_threads.size()) {
		for (std::size_t idx = 0; idx < m_threads.size(); idx++) {
			m_threads[idx]->join();
			delete m_threads[idx];
		}

		m_threads.clear();
	}

	m_run = true;

	for (std::size_t idx = 0; idx < m_nthreads; idx++)
		m_threads.push_back(new std::thread([&]() { do_work(); return; }));

	return;
}

void
threadpool_t::stop(void)
{
	std::size_t sz(m_threads.size());

	m_run = false;

	for (std::size_t idx = 0; idx < sz; idx++) {
		m_threads[idx]->join();
		delete m_threads[idx];
	}

	m_threads.clear();
	return;
}

bool
threadpool_t::finished(void)
{
	return true == m_done;
}