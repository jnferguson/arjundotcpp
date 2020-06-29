#include "config.hpp"

config_t::config_t(void)
	: m_url(""), m_output_file(""), m_delay(0), m_thread_count(2), m_wordlist_path(""),
	m_urls_path(""), m_get(true), m_post(false), m_headers_path(""),
	m_json(false), m_prefer_stability(false), m_include_data("")
{
	return;
}

bool
config_t::validate_state(void)
{
	if ((!m_url.length() && !m_urls_path.length()) || !m_wordlist_path.length()) {
		usage_message("The parameters --url or --urls and --wordlist-file are required and one or more was not specified");
		return false;
	}

	if (true == m_prefer_stability && 0 < m_delay)
		m_delay = 0;

	if (true == m_json && false == m_post) {
		usage_message("Specified that JSON formatted POST data and specified that HTTP GET should be used; conflicting directives received");
		return false;
	}

	return true;
}

config_t&
config_t::instance(void)
{
	static config_t instance;
	return instance;
}

void
config_t::usage(void)
{
	std::cerr << "arjun.cpp usage: " << std::endl << "[--help | -h] [--url | -u] <url> [--output-file | -o ] <output file> [--delay | -d ] <delay> ";
	std::cerr << "[--threads | -t] <thread count> [--wordlist-file | -f] <wordlist file> [--urls-file] <urls file> [--get] [--post] [--headers-file] <headers file> ";
	std::cerr << "[--json] [--stable] [--include-file] <include file>" << std::endl << std::endl;
	std::cerr << "[--help | -h]                              - Print this message and exit" << std::endl;
	std::cerr << "[--url | -u] <url>                         - target url" << std::endl;
	std::cerr << "[--output-file | -o ] <output file>        - path for the output file" << std::endl;
	std::cerr << "[--delay | -d ] <delay>                    - request delay, default is 0" << std::endl;
	std::cerr << "[--threads | -t] <thread count>            - number of threads; default is 2" << std::endl;
	std::cerr << "[--wordlist-file | -f] <wordlist file>     - wordlist path" << std::endl;
	std::cerr << "[--urls-file] <urls file>                  - file containing target urls" << std::endl;
	std::cerr << "[--get]                                    - use HTTP GET method; this is the default method" << std::endl;
	std::cerr << "[--post]                                   - use HTTP POST method" << std::endl;
	std::cerr << "[--headers-file] <headers file>            - add headers" << std::endl;
	std::cerr << "[--json]                                   - treat post data as json" << std::endl;
	std::cerr << "[--stable]                                 - prefer stability over speed" << std::endl;
	std::cerr << "[--include-file] <include file>            - include this data in every request" << std::endl;
	std::cerr << std::endl;
	return;
};

void
config_t::usage_message(std::string msg)
{
	std::cerr << msg << std::endl << std::endl;
	usage();
	return;
}

bool
config_t::parse_args(signed int ac, char** av)
{
	std::size_t arg_count(static_cast<std::size_t>(ac));
	std::size_t idx(1);

	if (5 > arg_count) {
		usage();
		return false;
	}

	do {
		if (! std::strncmp(av[idx], "--url", std::strlen("--url")) || ! std::strncmp(av[idx], "-u", 2)) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --url");
				return false;
			}

			m_url = av[++idx];
			continue;
		} else if (! std::strncmp(av[idx], "--help", std::strlen("--help")) || ! std::strncmp(av[idx], "-h", 2)) {
			usage();
			return false;

		} else if (! std::strncmp(av[idx], "--output-file", std::strlen("--output-file")) || ! std::strncmp(av[idx], "-o", 2)) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --output-file");
				return false;
			}

			m_output_file = av[++idx];
			continue;
		} else if (! std::strncmp(av[idx], "--delay", std::strlen("--delay")) || ! std::strncmp(av[idx], "-d", 2)) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --delay");
				return false;
			}

			m_delay = std::strtoul(av[++idx], nullptr, 10);
			continue;
		
		} else if (! std::strncmp(av[idx], "--threads", std::strlen("--threads")) || ! std::strncmp(av[idx], "-t", 2)) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --threads");
				return false;
			}

			m_thread_count = std::strtoul(av[++idx], nullptr, 10);
			continue;
		} else if (! std::strncmp(av[idx], "--wordlist-file", std::strlen("--wordlist-file")) || ! std::strncmp(av[idx], "-f", 2)) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --wordlist-file");
				return false;
			}

			m_wordlist_path = av[++idx];
			continue;
		} else if (! std::strncmp(av[idx], "--urls-file", std::strlen("--urls-file"))) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --urls-file");
				return false;
			}

			m_urls_path = av[++idx];
			continue;
		} else if (! std::strncmp(av[idx], "--get", std::strlen("--get"))) {
			m_get = true;
			m_post = false;
			continue;
		} else if (! std::strncmp(av[idx], "--post", std::strlen("--post"))) {
			m_post = true;
			m_get = false;
			continue;
		} else if (! std::strncmp(av[idx], "--headers-file", std::strlen("--headers-file"))) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --headers-file");
				return false;
			}

			m_headers_path = av[++idx];
			continue;
		} else if (! std::strncmp(av[idx], "--json", std::strlen("--json"))) {
			m_json = true;
			continue;
		} else if (! std::strncmp(av[idx], "--stable", std::strlen("--stable"))) {
			m_prefer_stability = true;
			continue;
		} else if (! std::strncmp(av[idx], "--include-file", std::strlen("--include-file"))) {
			if (idx + 1 >= ac) {
				usage_message("missing parameter to --include-file");
				return false;
			}

			m_include_data = av[++idx];
			continue;
		} else {
			std::cerr << "Unknown parameter: " << av[idx] << std::endl;
			usage();
			return false;
		}

	} while (++idx < ac);

	return validate_state();
}


const std::string 
config_t::target_url(void) const 
{
	return m_url;
}

void 
config_t::target_url(const std::string u) 
{
	m_url = u;
	return;
}

const std::string 
config_t::output_file_path(void) const 
{
	return m_output_file;
}

void 
config_t::output_file_path(const std::string of) 
{
	m_output_file = of;
	return;
}

const std::size_t 
config_t::delay(void) const 
{
	return m_delay;
}

void 
config_t::delay(const std::size_t d) 
{
	m_delay = d;
	return;
}

const std::size_t 
config_t::thread_count(void) const 
{
	return m_thread_count;
}

void 
config_t::thread_count(const std::size_t tc) 
{
	m_thread_count = tc;
	return;
}

const std::string 
config_t::wordlist_path(void) const 
{
	return m_wordlist_path;
}

void 
config_t::wordlist_path(const std::string wp) 
{
	m_wordlist_path = wp;
}

const std::string 
config_t::urls_path(void) const 
{
	return m_urls_path;
}

void 
config_t::urls_path(const std::string up) 
{
	m_urls_path = up;
	return;
}

const bool 
config_t::get(void) const 
{
	return m_get;
}

void 
config_t::get(const bool val) 
{
	m_get = val;
	return;
}

const bool 
config_t::post(void) const 
{
	return m_post;
}

void 
config_t::post(const bool val) 
{
	m_post = val;
	return;
}

const std::string 
config_t::headers_path(void) const 
{
	return m_headers_path;
}

void 
config_t::headers_path(const std::string hp) 
{
	m_headers_path = hp;
	return;
}

const bool 
config_t::json(void) const 
{
	return m_json;
}

void 
config_t::json(const bool val) 
{
	m_json = val;
	return;
}

const bool 
config_t::prefer_stability(void) const 
{
	return m_prefer_stability;
}

void 
config_t::prefer_stability(const bool val) 
{
	m_prefer_stability = val;
	return;
}

const std::string 
config_t::include_data_path(void) const 
{
	return m_include_data;
}

void 
config_t::include_data_path(const std::string idp) 
{
	m_include_data = idp;
	return;
}
