#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <mutex>
#include "http_common.hpp"

class globals_t
{
	private:
		globals_t(void);
	protected:
		std::string					m_known_good_body;
		std::string					m_known_good_plaintext;
		std::size_t					m_known_good_status;
		std::string					m_known_bad_body;
		std::string					m_known_bad_plaintext;
		std::size_t					m_known_bad_status;
		std::size_t					m_reflections;
		bool						m_same_html;
		bool						m_same_plaintext;

		mutable std::mutex			m_mutex;
		std::vector< std::string >	m_params;

	public:
		globals_t(globals_t const&) = delete;
		void operator=(globals_t const&) = delete;

		virtual ~globals_t(void) = default;

		static globals_t& instance(void);

		std::string known_good_body(void) const;
		void known_good_body(const std::string&);

		std::string known_good_plaintext(void) const;
		void known_good_plaintext(const std::string&);

		const std::size_t known_good_status(void) const;
		void known_good_status(const std::size_t);

		std::string known_bad_body(void) const;
		void known_bad_body(const std::string&);

		std::string known_bad_plaintext(void) const;
		void known_bad_plaintext(const std::string&);

		const std::size_t known_bad_status(void) const;
		void known_bad_status(const std::size_t);

		const std::size_t reflections(void) const;
		void reflections(const std::size_t);

		const bool same_html(void) const;
		void same_html(const bool);

		const bool same_plaintext(void) const;
		void same_plaintext(const bool);

		void add_found_parameter(const std::string&);
		const std::size_t found_parameters_size(void) const;
		std::string found_parameter_at(const std::size_t);
};

