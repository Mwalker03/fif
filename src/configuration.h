#pragma once

#include <stdio.h>
#include <string>

typedef struct _configuration_manager
{
    bool debug = 0;
    bool verbose = 0;
    bool show_errors = 0;
    bool to_file = 0;
    bool full_scan = 0;
    size_t file_size = 2097152;  // limit file size to 2MB
    std::string output_file_path;
}configuration_manager_t;

//extern configurationmanager config;
