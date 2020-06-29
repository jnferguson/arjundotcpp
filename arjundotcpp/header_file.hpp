#pragma once
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include "file.hpp"
#include "util.hpp"
#include "http_common.hpp"


bool parse_header_file(std::string path, header_map_t& hdrmap);
