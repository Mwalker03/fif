#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

#include "argsparser/argsparser.h"
#include "os/os.h"
#include "configuration.h"
#include "utilities.h"
#include "colors/colors.h"
#include "fif.h"

using namespace std;

int main(int argc, const char** argv)
{
	extern configuration_manager_t config;
	list<string> words_list;
	string startpoint = ".";

	parser p(argc, argv);
	if (p.has_kay("-d"))
		config.debug = true;
	if (p.has_kay("-e"))
		config.show_errors = true;
	if (p.has_kay("-v"))
		config.verbose = true;
	if (p.has_kay("-w"))
	{
		string value = p.get_value("-w");
		if (value.empty())
		{
			cperror((char *)"Fatal Error! -w has been sent without value");
			exit(EXIT_FAILURE);
		}
		words_list = parse_words_list(value);
	}
	if (p.has_kay("-W"))
	{
		string value = p.get_value("-W");
		if (value.empty())
		{
			cperror((char *)"Fatal Error! -W has been sent without value");
			exit(EXIT_FAILURE);
		}

		words_list = get_words_from_file(value);
	}
	if (p.has_kay("-o"))
	{
		string value = p.get_value("-o");
		if (value.empty())
		{
			cperror((char *)"Fatal Error! -o has been sent without value");
			exit(EXIT_FAILURE);
		}
		config.to_file = true;
		config.output_file_path = value;
	}
	if (p.has_kay("-l"))
	{
		string value = p.get_value("-o");
		if (value.empty())
		{
			cperror((char *)"Fatal Error! -o has been sent without value");
			exit(EXIT_FAILURE);
		}
		int n = atoi(value.c_str());
		if (n == 0 || n < 1 || n > 9999999)
			printf("value must be between 1 and 9999999");
		else
			config.file_size = n;
	}
	if (p.has_kay("-h"))
	{
		print_help();
		exit(EXIT_SUCCESS);
	}
	if (p.has_kay("-sp"))
	{
		string value = p.get_value("-sp");
		if (value.empty())
		{
			cperror((char *)"Fatal Error! -sp has been sent without value");
			exit(EXIT_FAILURE);
		}
		startpoint = value;
	}

	if (words_list.size() == 0)
		words_list = get_default_word_list();

	if (config.debug)
	{
		cprintf(color::blue, "\n%s\n", "debug:");
		printf("startpoint: %s\n", startpoint.c_str());
		printf("argc: %d\n", argc);
		for (int i = 0; i < argc; i++)
		{
			printf("argv[%d]: %s\n", i, argv[i]);
		}
	}
	
	printf("scan started from: %s\n", startpoint.c_str());
	scan_r(startpoint, words_list);
	printf("\n%s", "scan ended.\n");
	
	return EXIT_SUCCESS;
}
