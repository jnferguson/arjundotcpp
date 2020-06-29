#include "util.hpp"

bool
read_file(std::string path, std::string& output)
{
    input_file_t            infile;
    std::vector< uint8_t >  vec;
    std::string             ret("");

    if (false == infile.open(path)) {
        std::cerr << "Error opening file: '" << path << "'" << std::endl;
        return false;
    }

    if (false == infile.read(vec, infile.size())) {
        std::cerr << "Error reading file: '" << path << "'" << std::endl;
        return false;
    }

    output.clear();
    output.insert(output.end(), vec.begin(), vec.end());
    return true;
}

bool
tokenize_file(std::string contents, std::vector< std::string >& output, std::string re)
{
    std::regex nlre(re.c_str());
    std::sregex_token_iterator itr(contents.begin(), contents.end(), nlre, -1);
    std::sregex_token_iterator end;
    std::vector< std::string > ret(itr, end);

    output = ret;
    return true;
}

bool 
vector_has_element(const stringvector_t& vec, std::string elem)
{
    std::size_t sz(vec.size());

    for (std::size_t idx = 0; idx < sz; idx++)
        if (vec[idx] == elem)
            return true;

    return false;
}

// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
std::string
trim_whitespace(const std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');

    if (std::string::npos == first)
        return str;

    std::size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string 
random_hexstring(std::size_t length)
{
    rand_t		        rnd;
    std::stringstream   ss;

    ss.fill('0');
    for (std::size_t idx = 0; idx < length; idx++) {
        uint8_t byte(0);
        
        do {
            byte = rnd.byte();
        } while (! std::isalnum(byte));

        ss << std::hex << std::setw(1) << byte;
    }

    return ss.str();
}

std::string
random_decstring(std::size_t length)
{
    rand_t		        rnd;
    std::stringstream   ss;

    ss.fill('0');
    for (std::size_t idx = 0; idx < length; idx++) {
        uint8_t byte(0);

        do {
            byte = rnd.byte();
        } while (!std::isdigit(byte));

        ss << std::dec << std::setw(1) << byte;
    }

    return ss.str();
}

std::string
reverse_string(std::string str)
{
    std::string ret(str.rbegin(), str.rend());
    return ret;
}

std::size_t 
count_substring(std::string contents, std::string subs)
{
    std::size_t occurrences(0);
    std::string::size_type start(0);

    while (std::string::npos != (start = contents.find(subs, start))) {
        occurrences++;
        start += 1;
    }

    return occurrences;

    std::regex              re(subs.c_str());
    std::sregex_iterator    itr = std::sregex_iterator(contents.begin(), contents.end(), re);
    std::sregex_iterator    end = std::sregex_iterator();
    ptrdiff_t               ret(std::distance(itr, end));

    if (0 > ret)
        return 0;

    return ret;
}

std::string 
remove_tags(std::string src)
{
    std::regex  re("<[^<]*>");
    return std::regex_replace(src, re, "");
}