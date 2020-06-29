#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <iostream>

class config_t
{
	private:
		config_t(void);

	protected:
		std::string		m_url;
		std::string		m_output_file;
		std::size_t		m_delay;
		std::size_t		m_thread_count;
		std::string		m_wordlist_path;
		std::string		m_urls_path;
		bool			m_get;
		bool			m_post;
		std::string 	m_headers_path;
		bool			m_json;
		bool			m_prefer_stability;
		std::string		m_include_data;


		bool validate_state(void);

	public:
		config_t(config_t const&) = delete;
		void operator=(config_t const&) = delete;

		virtual ~config_t(void) = default;

		static config_t& instance(void);

		void usage(void);
		void usage_message(std::string msg);

		bool parse_args(signed int ac, char** av);

		virtual const std::string target_url(void) const;
		virtual void target_url(const std::string);

		virtual const std::string output_file_path(void) const;
		virtual void output_file_path(const std::string);

		virtual const std::size_t delay(void) const;
		virtual void delay(const std::size_t);

		virtual const std::size_t thread_count(void) const;
		virtual void thread_count(const std::size_t);

		virtual const std::string wordlist_path(void) const;
		virtual void wordlist_path(const std::string);

		virtual const std::string urls_path(void) const;
		virtual void urls_path(const std::string);

		virtual const bool get(void) const;
		virtual void get(const bool);

		virtual const bool post(void) const;
		virtual void post(const bool);

		virtual const std::string headers_path(void) const;
		virtual void headers_path(const std::string);

		virtual const bool json(void) const;
		virtual void json(const bool);

		virtual const bool prefer_stability(void) const;
		virtual void prefer_stability(const bool);

		virtual const std::string include_data_path(void) const;
		virtual void include_data_path(const std::string);
};

