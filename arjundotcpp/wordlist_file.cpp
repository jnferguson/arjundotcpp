#include "wordlist_file.hpp"

bool
parse_wordlist_file(std::string path, stringvector_t& wordlist)
{
    std::string					contents("");
    std::vector< std::string >	tokenized_file;

    if (false == read_file(path, contents)) {
        std::cerr << "Error reading wordlist file: '" << path << "'" << std::endl;
        return false;
    }

    if (false == tokenize_file(contents, tokenized_file, "\\r?\\n+")) {
        std::cerr << "Error tokenizing wordlist file: '" << path << "'" << std::endl;
        std::cerr << "Leading and trailing whitespace is removed" << std::endl;
        return false;
    }

    if (wordlist.size() > std::numeric_limits< std::size_t >::max() - tokenized_file.size()) {
        std::cerr << "Overly large number of words in wordlist file encountered" << std::endl;
        return false;
    }

    wordlist.reserve(wordlist.size() + tokenized_file.size());

    for (std::vector< std::string >::iterator itr = tokenized_file.begin(); itr != tokenized_file.end(); itr++) {
        std::string word(trim_whitespace(*itr));
        wordlist.push_back(word);
    }

    return true;
}
