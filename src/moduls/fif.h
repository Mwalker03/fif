#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include <vector>
#include <string>
#include <list>

#include "../configuration.h"
#include "../colors/colors.h"
#include "../os/os.h"
#include "../strings.h"
#include "../utilities.h"

using namespace std;

typedef struct _sensitive_data
{
	string filename;
	int index;
	string snipcode;
}sen_data;

typedef struct _local_config
{
	bool common_places = false;
	bool full_scan = false;
	string cp; // windows or linux common places
}local_config;

struct local
{
public:
	static void scan_r(string root, list<string> pattrens);
	static local_config parse_args(parser *p);
	static void print_help();
    static void test();
private:
	static void find_sensitive_v2(string fname, list<string> pattrens);
};
