#include "configuration.h"

#include "strings.h"
#include "utilities.h"
#include "colors/colors.h"

configuration_manager_t global_config;

void configuration_manager_t::parse_args(parser& p)
{
	if (p.has_kay("-d"))
		this->debug = true;
	if (p.has_kay("-e"))
		global_config.show_errors = true;
	if (p.has_kay("-v"))
		global_config.verbose = true;

	if (p.has_kay("-o"))
	{
		string value = get_arg_value(&p, "-o");
		global_config.to_file = true;
		// just a small hack for reletive path
#ifdef _WIN32
		if (!strings::contains(value, ":"))
		{
			string exec_path = get_exec_directory();
			value = strings::vformat("%s\\%s", 
							exec_path.c_str(), value.c_str());
			
			if (config.debug)
			{
				cprintf(color::blue, "\n%s\n", "debug:");
				printf("exec_path: %s\nvalue: %s",
					exec_path.c_str(), value.c_str());
			}
		}
#else
		if (!strings::contains(value, "/"))
		{
			string exec_path = get_exec_directory();
			value = strings::vformat("%s/%s",
				exec_path.c_str(), value.c_str());
			
			if (global_config.debug)
			{
				cprintf(color::blue, "\n%s\n", "debug:");
				printf("exec_path: %s\nvalue: %s",
					exec_path.c_str(), value.c_str());
			}
		}
#endif

		global_config.output_file_path = value;
	}

	if (p.has_kay("-l"))
	{
		string value = get_arg_value(&p, "-l");
		int n = atoi(value.c_str());
		if (n == 0 || n < 1 || n > 9999999)
			printf("value must be between 1 and 9999999");
		else
			global_config.file_size = n;
	}
	if (p.has_kay("-h"))
	{
		print_help();
		exit(EXIT_SUCCESS);
	}
	if (p.has_kay("-sp"))
	{
		string value = get_arg_value(&p, "-sp");
		global_config.startpoint = value;
	}
		if (p.has_kay("-w"))
	{
		string value = get_arg_value(&p, "-w");
		global_config.words_list = parse_words_list(value);
	}
	if (p.has_kay("-W"))
	{
		string value = get_arg_value(&p, "-W");
		global_config.words_list = get_words_from_file(value);
	}
}
