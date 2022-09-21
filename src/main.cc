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

string get_args_value(parser* p, string arg)
{
	string value = p->get_value(arg.c_str());
	if (value.empty())
	{
		string format = strings::vformat("Fatal Error! %s has been sent without value",
			arg.c_str());
		cperror((char*)format.c_str());
		exit(EXIT_FAILURE);
	}
	else
		return value;
}
int main(int argc, const char** argv)
{
	extern configuration_manager_t config;
	list<string> words_list;
	string startpoint = ".";
	
	words_list = get_default_word_list();

	parser p(argc, argv);
	if (p.has_kay("-d"))
		config.debug = true;
	if (p.has_kay("-e"))
		config.show_errors = true;
	if (p.has_kay("-v"))
		config.verbose = true;
	if (p.has_kay("-w"))
	{
		string value = get_args_value(&p, "-w");
		words_list = parse_words_list(value);
	}
	if (p.has_kay("-W"))
	{
		string value = get_args_value(&p, "-W");
		words_list = get_words_from_file(value);
	}
	if (p.has_kay("-o"))
	{
		string value = get_args_value(&p, "-o");
		config.to_file = true;
		config.output_file_path = value;
	}
	if (p.has_kay("-l"))
	{
		string value = get_args_value(&p, "-l");
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
		string value = get_args_value(&p, "-sp");
		startpoint = value;
	}

	if (p.has_kay("--common-places"))
	{
		string value = get_args_value(&p, "--common-places");
		if (strings::to_lower(value) == "windows")
		{
			scan_r("C:\\ProgramData", words_list);
			scan_r("C:\\Users", words_list);
			scan_r("C:\\Inetpub", words_list);
		}
		else if (strings::to_lower(value) == "linux")
		{
			printf("Linux common places will come soon ..");
			exit(EXIT_FAILURE);
		}
	}
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
