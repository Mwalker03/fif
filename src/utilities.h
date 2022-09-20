#pragma once

#include <sstream>
#include <list>
#include <string>

using namespace std;

list<string> parse_words_list(string wordlist);
list<string> get_default_word_list();
list<string> get_words_from_file(string path);
void print_help();
