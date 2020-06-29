#include "header_file.hpp"

bool 
parse_header_file(std::string path, header_map_t& hdrmap)
{
    std::string                 contents("");
    std::vector< std::string >  tokenized_file;

    if (false == read_file(path, contents))
        return false;
    if (false == tokenize_file(contents, tokenized_file))
        return false;

    for (std::vector< std::string >::iterator itr = tokenized_file.begin(); itr != tokenized_file.end(); itr++) {
        std::string line(*itr);
        std::regex  nvre("(.*):(.*)");
        std::smatch matches;

        if (std::regex_match(line, matches, nvre)) {
            std::string n(""), v("");
            if (false == matches[1].matched || false == matches[2].matched) {
                std::cerr << "Malformed header file encountered; file should be 1 header per line in the format of <header name> : <header value>." << std::endl;
                std::cerr << "All leading and trailing whitespace is trimmed from the two values" << std::endl;
                std::cerr << "In the event of overlapped header values, the last one encountered overwrites prior instances" << std::endl;
                return false;
            }

            n = trim_whitespace(matches[1].str());
            v = trim_whitespace(matches[2].str());
            hdrmap[n] = v;
        }
    }

    return true;
}
