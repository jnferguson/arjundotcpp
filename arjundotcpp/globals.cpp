#include "globals.hpp"

globals_t::globals_t(void)
	: m_known_good_body(""), m_known_good_plaintext(""), m_known_good_status(0), m_known_bad_body(""), 
	m_known_bad_plaintext(""), m_known_bad_status(0), m_reflections(0), m_same_html(false), m_same_plaintext(false)
{
	return;
}

globals_t&
globals_t::instance(void)
{
	static globals_t instance;
	return instance;
}

std::string 
globals_t::known_good_body(void) const
{
	return m_known_good_body;
}

void
globals_t::known_good_body(const std::string& v)
{
	m_known_good_body = v;
	return;
}

std::string 
globals_t::known_good_plaintext(void) const
{
	return m_known_good_plaintext;
}

void
globals_t::known_good_plaintext(const std::string& v) 
{
	m_known_good_plaintext = v; 
	return;
}

const std::size_t 
globals_t::known_good_status(void) const
{
	return m_known_good_status;
}

void
globals_t::known_good_status(const std::size_t v) 
{
	m_known_good_status = v; 
	return;
}

std::string 
globals_t::known_bad_body(void) const
{
	return m_known_bad_body;
}

void
globals_t::known_bad_body(const std::string& v)
{
	m_known_bad_body = v; 
	return;
}

std::string 
globals_t::known_bad_plaintext(void) const
{
	return m_known_bad_plaintext;
}

void
globals_t::known_bad_plaintext(const std::string& v) 
{
	m_known_bad_plaintext = v; 
	return;
}

const std::size_t 
globals_t::known_bad_status(void) const
{
	return m_known_bad_status;
}

void
globals_t::known_bad_status(const std::size_t v) 
{
	m_known_bad_status = v; 
	return;
}

const std::size_t 
globals_t::reflections(void) const
{
	return m_reflections;
}

void
globals_t::reflections(const std::size_t v) 
{
	m_reflections = v; 
	return;
}

const bool 
globals_t::same_html(void) const
{
	return m_same_html;
}

void 
globals_t::same_html(const bool v)
{
	m_same_html = v;
	return;
}

const bool 
globals_t::same_plaintext(void) const
{
	return m_same_plaintext;
}

void 
globals_t::same_plaintext(const bool v)
{
	m_same_plaintext = v;
	return;
}

void 
globals_t::add_found_parameter(const std::string& v)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	m_params.push_back(v);
	return;
}

const std::size_t 
globals_t::found_parameters_size(void) const
{
	std::lock_guard< std::mutex >	lck(m_mutex);
	std::size_t						ret(m_params.size());

	return ret;
}

std::string 
globals_t::found_parameter_at(const std::size_t idx)
{
	std::lock_guard< std::mutex > lck(m_mutex);

	if (idx >= m_params.size())
		throw std::invalid_argument("globals_t::found_parameter_at(): invalid index specified");
	
	return m_params[idx];
}