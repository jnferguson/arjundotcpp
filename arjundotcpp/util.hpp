#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <regex>
#include <sstream>
#include <iomanip>
#include "rand.hpp"
#include "file.hpp"

typedef std::vector< std::string > stringvector_t;

bool read_file(std::string path, std::string& output);
bool tokenize_file(std::string contents, std::vector< std::string >& output, std::string re = "\\n+");
bool vector_has_element(const stringvector_t&, std::string);

std::string trim_whitespace(const std::string& str);
std::string random_hexstring(std::size_t length);
std::string random_decstring(std::size_t length);
std::string reverse_string(std::string str);
std::size_t count_substring(std::string contents, std::string subs);
std::string remove_tags(std::string);
