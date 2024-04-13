#pragma once

#include <sstream>
#include <list>
#include <string>
#include <regex>
#include <vector>

#include "argsparser/argsparser.h"

using namespace std;

typedef struct _finder
{
    typedef struct _sensitive_data_t
    {
        int     line_number;
        string  file_name;
        string  full_path;
        string  snipshot;
    }sen_data_t;

public:
    static vector<sen_data_t> find_sensitive_data(const string& full_path,
                                            const string& text, 
                                            const list<string>& patterns);
}finder;

typedef struct _helper
{
public:
    static string combine_patterns(const list<string>& patterns);
    static void print_result(const vector<finder::sen_data_t>& results);
    static bool is_digit(const string& str);
    static bool is_ext_ignored(const string& fname);
    static void print_help();
    static void log(const string& fpath,
                    const vector<finder::sen_data_t>& data);


private:
    static FILE* f_open(string filepath, string mode);
}helper;

 
string get_arg_value(parser *p, const char *arg);
string get_exec_directory();
list<string> parse_words_list(string wordlist);
list<string> get_default_word_list();
list<string> get_words_from_file(string path);

