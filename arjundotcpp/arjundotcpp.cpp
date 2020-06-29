// arjundotcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include "sock.hpp"
#include "http_request.hpp"
#include "http_client_socket.hpp"
#include "config.hpp"
#include "header_file.hpp"
#include "param_file.hpp"
#include "url_file.hpp"
#include "wordlist_file.hpp"
#include "requester.hpp"
#include "globals.hpp"
#include "threadpool.hpp"

bool
http_parameter_vector_has_element(const http_parameters_t& vec, const std::string& elem)
{
    std::size_t sz(vec.size());

    for (std::size_t idx = 0; idx < sz; idx++)
        if (vec[idx]->parameter_name() == elem)
            return true;

    return false;
}

bool
process_response(http_response_t* resp, const http_parameters_t& parameters, const http_parameters_t& include)
{
    if (nullptr == resp)
        return false;

    if (429 == resp->status()) {
        if (true == config_t::instance().prefer_stability()) {
            std::cout << "Hit rate limit, stabilizing the connection" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(30));
        } else {
            std::cout << "Target has rate limiting in place, please use the --stable switch";
            throw std::runtime_error("process_response(): Target has rate limiting in place and --stable was not specified");
        }
    }

    if (resp->status() != globals_t::instance().known_bad_status())
        return true;
    else if (true == globals_t::instance().same_html() && resp->body().length() != globals_t::instance().known_bad_body().length())
        return true;
    else if (true == globals_t::instance().same_plaintext() && remove_tags(resp->body()).length() != globals_t::instance().known_bad_plaintext().length())
        return true;
    else {
        std::size_t psz(parameters.size());

        for (std::size_t idx = 0; idx < psz; idx++) {
            std::string val(parameters[idx]->parameter_name());

            if (false == http_parameter_vector_has_element(include, val))
                if (count_substring(resp->body(), parameters[idx]->parameter_value()) != globals_t::instance().reflections())
                    return true;
        }
    }

    return false;
}

void
heuristic(std::string response, stringvector_t& params)
{
    std::regex                      re("<form[\\s\\S]*?</form[\\s\\S]*?>",std::regex::icase); // ,  | std::regex::icase);
    std::sregex_iterator            forms_begin = std::sregex_iterator(response.begin(), response.end(), re);
    std::sregex_iterator            forms_end = std::sregex_iterator();
    std::regex                      js_re("var\\s+([^=]+)\\s*=\\s*['\"`]['\"`]");
    std::sregex_iterator            js_begin = std::sregex_iterator(response.begin(), response.end(), js_re);
    std::sregex_iterator            js_end = std::sregex_iterator();
    stringvector_t                  tmp_vec;

    for (std::sregex_iterator forms_itr = forms_begin; forms_itr != forms_end; forms_itr++) {
        std::string             form(forms_itr->str());
        std::regex              method_re("method=['\"](.*?)['\"]", std::regex::icase);
        std::smatch             method_smatch;
        std::regex              input_re("<input[\\s\\S]*?>", std::regex::icase);
        std::sregex_iterator    inputs_begin = std::sregex_iterator(form.begin(), form.end(), input_re);
        std::sregex_iterator    inputs_end = std::sregex_iterator();

        if (std::regex_search(form, method_smatch, method_re)) {
            for (std::sregex_iterator inputs_itr = inputs_begin; inputs_itr != inputs_end; inputs_itr++) {
                std::string         input(inputs_itr->str());
                std::regex          name_re("name=['\"](.*?)['\"]", std::regex::icase);

                if (std::regex_search(input, name_re)) {
                    std::smatch input_name_smatch;
                    std::regex_search(input, input_name_smatch, name_re);
                    std::string input_name_value(input_name_smatch[1].str());

                    if (true == vector_has_element(tmp_vec, input_name_value))
                        continue;

                    tmp_vec.push_back(input_name_value);
                    std::cout << "Heuristic found a potential " << method_smatch[1].str() << " parameter: " << input_name_smatch[1].str() << std::endl;
                    std::cout << "Prioritizing it" << std::endl;    
                }
            }
        }
    }

    for (std::sregex_iterator js_itr = js_begin; js_itr != js_end; js_itr++) {
        std::string var_name((*js_itr)[1].str());

        if (true == vector_has_element(tmp_vec, var_name))
            continue;

        tmp_vec.push_back(var_name);
        std::cout << "Heuristic found a potential parameter: " << var_name << std::endl;
        std::cout << "Prioritizing it" << std::endl;
    }

    if (0 != tmp_vec.size()) {
        for (std::size_t idx = 0; idx < params.size(); idx++)
            tmp_vec.push_back(params[idx]);

        params = tmp_vec;
        tmp_vec.clear();
    }

    return;
}

bool
initialize(stringvector_t& wordlist, std::string url, http_parameters_t parameters, header_map_t headers) //, bool is_get, std::size_t delay)
{
    http_response_t*                known_good(nullptr);
    http_response_t*                known_bad(nullptr);
    http_parameters_t               bad_params;
    string_pair_request_data_t*     bad_param_pair(new string_pair_request_data_t);
    std::size_t                     reflections(0);
    std::size_t                     new_length(0);
    std::string                     plain_text("");
    std::size_t                     plain_text_length(0);
    stringvector_t                  sv;

    if (false == requester_t::stabilize(url, headers)) {
        std::cerr << "Failed connecting to " << url << std::endl;
        return false;
    }

    std::cout << "Analysing the content of the webpage" << std::endl;
    known_good = requester_t::request(url, parameters, headers, config_t::instance().get(), config_t::instance().delay());

    if (nullptr == known_good) {
        std::cerr << "Error performing initial request; this is fatal" << std::endl;
        return false;
    }

    globals_t::instance().known_good_body(known_good->body());
    globals_t::instance().known_good_plaintext(remove_tags(known_good->body()));
    globals_t::instance().known_good_status(known_good->status());

    bad_param_pair->parameter_name(random_hexstring(8));
    bad_param_pair->parameter_value(reverse_string(bad_param_pair->parameter_name()));
    bad_params.push_back(bad_param_pair);

    known_bad = requester_t::request(url, bad_params, headers, config_t::instance().get(), config_t::instance().delay());

    if (nullptr == known_bad) {
        std::cerr << "Error retrieving known-bad parameters; this is a fatal error" << std::endl;
        return false;
    }

    reflections = count_substring(known_bad->body(), bad_param_pair->parameter_value());
    std::cout << "Reflections " << reflections << std::endl;
    std::cout << "Response Code " << known_bad->status() << std::endl;

    globals_t::instance().known_bad_body(known_bad->body());
    globals_t::instance().known_bad_plaintext(remove_tags(known_bad->body()));
    globals_t::instance().known_bad_status(known_bad->status());
    globals_t::instance().reflections(reflections);

    //new_length              = original_response->body().length();
    //plain_text              = remove_tags(original_response->body());
    //plain_text_length       = plain_text.length();
    //factors.sameHTML        = false;
    //factors.samePlainText   = false;

    if (globals_t::instance().known_good_body().length() == globals_t::instance().known_bad_body().length())
        globals_t::instance().same_html(true);
    else
        globals_t::instance().same_html(false);

    if (globals_t::instance().known_good_plaintext().length() == globals_t::instance().known_bad_plaintext().length())
        globals_t::instance().same_plaintext(true);
    else
        globals_t::instance().same_plaintext(false);

    /*if (first_response->body().length() == new_length)
        factors.sameHTML = true;
    else if (remove_tags(first_response->body()).length() == plain_text_length)
        factors.samePlainText = true;
        */

    std::cout << "Parsing webpage for potential parameters" << std::endl;

    heuristic(known_good->body(), wordlist);
    return true;
}

signed int
main(signed int ac, char** av)
{
    header_map_t        headers;
    http_parameters_t   parameters;
    stringvector_t      urls;
    stringvector_t      wordlist;
    std::map< std::string, void* > final_result;

    headers.clear();
    parameters.clear();
    urls.clear();
    wordlist.clear();

    if (false == config_t::instance().parse_args(ac, av))
        return EXIT_FAILURE;

    if (config_t::instance().headers_path().length()) {
        if (false == parse_header_file(config_t::instance().headers_path(), headers))
            return EXIT_FAILURE;
    } else {
        headers["Cache-Control"]                = "no-cache";
        headers["Connection"]                   = "close";
        headers["Pragma"]                       = "no-cache";
        headers["User-Agent"]                   = "Mozilla/5.0 (X11; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0";
        headers["Accept"]                       = "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
        headers["Accept-Language"]              = "en-US,en;q=0.5";
        headers["Upgrade-Insecure-Requests"]    = "1";
    }

    if (true == config_t::instance().json())
        headers["Content-Type"] = "application/json";

    if (config_t::instance().include_data_path().length()) {
        if (false == parse_param_file(config_t::instance().include_data_path(), parameters))
            return EXIT_FAILURE;
    }

    if (config_t::instance().target_url().length())
        urls.push_back(config_t::instance().target_url());

    if (config_t::instance().urls_path().length()) {
        if (false == parse_url_file(config_t::instance().include_data_path(), urls))
            return EXIT_FAILURE;
    }

    if (config_t::instance().wordlist_path().length()) {
        if (false == parse_wordlist_file(config_t::instance().wordlist_path(), wordlist))
            return EXIT_FAILURE;
    }

    if (urls.empty()) {
        std::cerr << "No target URLs specified" << std::endl;
        return EXIT_FAILURE;
    }

    if (wordlist.empty()) {
        std::cerr << "No parameter names specified / empty word list encountered" << std::endl;
        return EXIT_FAILURE;
    }

    for (std::size_t idx = 0; idx < urls.size(); idx++) {
        std::cout << "Scanning " << urls[idx] << std::endl;

        if (false == initialize(wordlist, urls[idx], parameters, headers))
            continue;
        {
            std::function< bool(http_response_t*, const http_parameters_t&, const http_parameters_t&)>  fptr(&::process_response);
            threadpool_t                                                                                tp(urls[idx], parameters, headers, wordlist);

            tp.set_process_response(fptr);
            tp.start();
            while (false == tp.finished())
                std::this_thread::sleep_for(std::chrono::seconds(1));

            std::cout << "Found " << globals_t::instance().found_parameters_size() << " potential parameters" << std::endl;
            for (std::size_t found_idx = 0; found_idx < globals_t::instance().found_parameters_size(); found_idx++)
                std::cout << "Valid parameter found: " << globals_t::instance().found_parameter_at(found_idx) << std::endl;

        }
    }

     return EXIT_SUCCESS;
}
