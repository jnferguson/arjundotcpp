#include "url_file.hpp"

bool
parse_url_file(std::string path, stringvector_t& urls)
{
    std::string					contents("");
    std::vector< std::string >	tokenized_file;

    if (false == read_file(path, contents)) {
        std::cerr << "Error reading URL file: '" << path << "'" << std::endl;
        return false;
    }

    if (false == tokenize_file(contents, tokenized_file, "\\r?\\n+")) {
        std::cerr << "Error tokenizing URL file: '" << path << "'" << std::endl;
        std::cerr << "Leading and trailing whitespace is removed" << std::endl;
        return false;
    }

    if (urls.size() > std::numeric_limits< std::size_t >::max() - tokenized_file.size()) {
        std::cerr << "Overly large URLs file encountered" << std::endl;
        return false;
    }

    urls.reserve(urls.size() + tokenized_file.size());

    for (std::vector< std::string >::iterator itr = tokenized_file.begin(); itr != tokenized_file.end(); itr++) {
        std::string url(trim_whitespace(*itr));
        urls.push_back(url);
    }

    return true;
}

