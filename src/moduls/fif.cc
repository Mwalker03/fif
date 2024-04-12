#include "fif.h"
#include <regex>

#ifdef WIN32_LEAN_AND_MEAN
	#include <direct.h>
#else
	#include <dirent.h>
	#include <limits.h>
	#include  <unistd.h>
	#include <sys/stat.h>
#endif

extern configuration_manager_t config;

bool is_valid_password(const string& password) {
	std::regex pattern("^[a-zA-Z0-9][a-zA-Z0-9!@#$%^&*()_=\\[\\]{};':\"\\\\|.\\/?]{4,25}$");
	bool is_match = std::regex_match(password, pattern);
	return is_match;
}

bool is_valid_username(const string& username) {
	std::regex pattern("^[a-zA-Z][a-zA-Z0-9.]*$");
	bool is_match = std::regex_match(username, pattern);
	return is_match;
}

FILE* f_open(string filepath, string mode)
{
	FILE* fp = fopen(filepath.c_str(), mode.c_str());
	if (fp == NULL)
	{
		if (global_config.show_errors)
		{
			if (strings::contains(mode, "a") || strings::contains(mode, "w"))
			{
				cperror((char*)"Failed to open %s for writing (%d: %s)\n",
					filepath.c_str(), errno, strerror(errno));
			}
			else
			{
				cperror((char*)"Failed to open %s for reading (%d: %s)\n",
					filepath.c_str(), errno, strerror(errno));
			}
		}
	}
	return fp;
}
void write_to_file(string str, string filepath)
{
	FILE* fp = f_open(filepath.c_str(), "a+");
	str += "\n"; // making sure line is finished
	fprintf(fp, "%s", str.c_str());
	fclose(fp);
}

bool is_gold(string patt, string sub)
{
	// Divide the result to key-value pair
	string key = patt;
	string value = sub;
	value = value.replace(0, patt.length(), "");
	value = strings::trim(value);
	if (strings::contains(key, "us"))
	{
		bool is_valid_un = is_valid_username(value);
		if (!is_valid_un)
			return false;
	}
	else if (strings::contains(key, "p") || 
			 strings::contains(key, "se"))
	{
		if (strings::contains(key, "secret") && 
			strings::contains(value, "secret"))
			return false;
		
		bool is_valid_ps = is_valid_password(value);
		if (!is_valid_ps)
			return false;
	}
	return true;
}
void local::find_sensitive_v2(string fname, list<string> pattrens)
{
	FILE* fp = f_open(fname.c_str(), "r");
	if (fp == NULL)
		return;

	char buf[10240] = { 0 };
	int line_counter = 1;
	do
	{
		if (fgets(buf, 10239, fp) != NULL)
		{
			string line(buf);
			line = strings::to_lower(line);
			sen_data sdata = {};
			for (string patt : pattrens)
			{
				// ignore nonsense comparison
				if (line.length() < patt.length())
					continue;

				size_t sindex = line.find(patt);
				if (sindex != string::npos)
				{
					string sub = line.substr(sindex, 25);
					if (!global_config.full_scan)
						if (!is_gold(patt, sub))
							continue;

					printf("file %s ", fname.c_str());
					cprintf(color::blue, "line ");
					printf("%d: ", line_counter);
					cprintf(color::green, "%s\n", sub.c_str());
					if (global_config.to_file)
					{
						string format = strings::vformat("file: %s Line: %d %s", 
															fname.c_str(),
															line_counter, 
															sub.c_str());
						write_to_file(format, global_config.output_file_path);
					}
				}
			}
		}
		if (strcmp(buf, "") == 0)
			break;
		if (strcmp(buf, "\000") == 0)
			break;

		line_counter++;
	} while (!feof(fp));

	fclose(fp);
}

bool is_c_dir(string dir_name)
{
	static list<string> c_dirs = {
	"Microsoft Visual Studio", "Adobe",
	"Bonjour", "Microsoft Office", "VMware",
	"Windows Media Player", "Windows Photo Viewer",
	"Android Studio", "Microsoft Web Tools",
	"Microsoft.NET", "Nmap", "NuGet", "Microsoft SDKs",
	"Windows Kits", "VirtualBox", "AspNetCore", 
	"VisualStudio", "Python27"
	};

	for (string name : c_dirs)
	{
		if (strings::contains(
			strings::to_lower(dir_name), 
			strings::to_lower(name))
		)
			return true;
	}
	return false;
}

void local::scan_r(string root, list<string> pattrens)
{
	list<fileentry *> files;
	string filebuff;
	sen_data sdata;
	static list<string> ignored = {
	"exe", "css", "doc", "docx" "xls", "xlsx",
	"ppt", "pptx", "odt", "pdf", "ods",
	"mp3", "mp4", "jpg", "jpeg", "png",
	"avi", "wav", "zip", "7z", "rar", "tar.gz",
	"bin", "iso", "mdb", "mdbx", ".tar", "com",
	"msi", "ico", "tif", "tiff", "mkv", "mov",
	"mpg", "mpeg", "swf", "wmv"
	};
	
	files = os::get_directory(root, global_config.file_size, 
								ignored, global_config.show_errors);

	for (fileentry *entry : files)
	{
		if (global_config.verbose)
			cprintf(color::mangeta,
				"entry.path: %s\n", entry->path.c_str());

		if (entry->isfile)
			local::find_sensitive_v2(entry->path, pattrens);
		else
		{
			if(global_config.full_scan)
				local::scan_r(entry->path, pattrens);
			else
			{
				if (is_c_dir(entry->path))
					continue;
				else
					local::scan_r(entry->path, pattrens);
			}
		}

		delete entry;
	}	
}

local_config local::parse_args(parser *p)
{
	local_config lc;
	if(p->has_kay("--full-scan"))
		global_config.full_scan = true;

	if(p->has_kay("--common-places"))
	{
		string value = get_arg_value(p, "--common-places");
		lc.common_places = true;
		lc.cp = value;
	}	
	return lc;
}
