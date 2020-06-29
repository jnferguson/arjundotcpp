#include "param_file.hpp"

bool
parse_param_file(std::string path, http_parameters_t& params)
{
    std::string					contents("");
    std::vector< std::string >	tokenized_file;

    if (false == read_file(path, contents)) {
        std::cerr << "Error reading parameter file: '" << path << "'" << std::endl;
        return false;
    }

    if (false == tokenize_file(contents, tokenized_file, "\\r?\\n+")) {
        std::cerr << "Error tokenizing parameter file: '" << path << "'" << std::endl;
        std::cerr << "Leading and trailing whitespace is removed" << std::endl;
        return false;
    }

    for (std::vector< std::string >::iterator itr = tokenized_file.begin(); itr != tokenized_file.end(); itr++) {
        std::string line(*itr);
        std::regex  nvre("(.*)=(.*)");
        std::smatch matches;

        if (std::regex_match(line, matches, nvre)) {
            std::string n(""), v("");
            if (false == matches[1].matched || false == matches[2].matched) {
                std::cerr << "Malformed parameter file encountered; file should be 1 variable per line in the format of <parameter name> = <parameter value>." << std::endl;
                std::cerr << "All leading and trailing whitespace is trimmed from the two values" << std::endl;
                return false;
            }

            n = trim_whitespace(matches[1].str());
            v = trim_whitespace(matches[2].str());
            params.push_back(new string_pair_request_data_t(n, v));
        }
    }

    return true;
}
