﻿#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <fstream>
#include <sstream>

#include <inttypes.h>
#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include <smb2/libsmb2-raw.h>

#include "argsparser/argsparser.h"
#include "os/os.h"
#include "configuration.h"
#include "utilities.h"
#include "colors/colors.h"
#include "moduls/fif.h"
#include "moduls/smb.h"


using namespace std;

void print_debug(int argc, const char** argv)
{
	cprintf(color::blue, "\n%s\n", "debug:");
	printf("argc: %d\n", argc);
	for (int i = 0; i < argc; i++)
	{
		printf("argv[%d]: %s\n", i, argv[i]);
	}
}

int main(int argc, const char** argv)
{
	//extern configuration_manager_t config;

	parser p(argc, argv);
	global_config.words_list = get_default_word_list();
	global_config.parse_args(p);

	if (global_config.debug)
		print_debug(argc, argv);

	if (p.has_kay("smb"))
	{
		smb_config_t smb_conf = smb::parse_smb_args(&p);
		puts("Scan started.");
		smb::scan_r(smb_conf.start_point, smb_conf);
		smb::free();
		puts("Scan finished.");

		return EXIT_SUCCESS;
	}
	else if(p.has_kay("local"))
	{
		local_config lcl_conf = local::parse_args(&p);
		if (strings::to_lower(lcl_conf.cp) == "windows")
		{
			printf("%s", "Scanning common windows places: \n");
			local::scan_r("C:\\ProgramData", global_config.words_list);
			local::scan_r("C:\\Users", global_config.words_list);
			local::scan_r("C:\\Inetpub", global_config.words_list);
			printf("\n%s", "\n\nScan ended.\n");		
		}
		else if (strings::to_lower(lcl_conf.cp) == "linux")
		{
			printf("Linux common places will come soon ..");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("scan started from: %s\n", 
					global_config.startpoint.c_str());
			
			local::scan_r(global_config.startpoint, 
							global_config.words_list);
							
			printf("\n%s", "scan ended.\n");		
		}		
	}
	else
	{
		helper::print_help();
		printf("%s", "You must pick a modul: [local], [smb]\n");
		exit(EXIT_FAILURE);
	}
	
	return EXIT_SUCCESS;
}
