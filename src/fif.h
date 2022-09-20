#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include <vector>
#include <string>
#include <list>

#include "configuration.h"
#include "colors/colors.h"
#include "os/os.h"
#include "strings.h"

using namespace std;

typedef struct sensitive_data
{
	string filename;
	int index;
	string snipcode;
}sen_data;


void scan_r(string root, list<string> pattrens);
void find_sensitive_v2(string fname, list<string> pattrens);
