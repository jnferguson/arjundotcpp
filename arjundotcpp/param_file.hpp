#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include "util.hpp"
#include "http_common.hpp"

bool parse_param_file(std::string path, http_parameters_t& params);

