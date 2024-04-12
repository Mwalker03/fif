#pragma once

#include <stdio.h>
#include <string>
#include <list>

#include "argsparser/argsparser.h"

typedef struct _configuration_manager
{
    bool debug = 0;
    bool verbose = 0;
    bool show_errors = 0;
    bool to_file = 0;
    bool full_scan = 0;
	string startpoint = ".";
	list<string> words_list;
    size_t file_size = 2097152;  // limit file size to 2MB
    string output_file_path;
    void parse_args(parser& p);
}configuration_manager_t;

extern configuration_manager_t global_config;
