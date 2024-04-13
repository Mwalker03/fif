#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm> 

#include "utilities.h"
#include "colors/colors.h"
#include "strings.h"
#include "configuration.h"

#ifdef WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <unistd.h>
#endif


FILE* helper::f_open(string filepath, string mode)
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

void helper::log(const string& fpath, 
				 const vector<finder::sen_data_t>& data)
{
	FILE* fp = f_open(fpath.c_str(), "a+");

	for(const finder::sen_data_t& d : data)
	{
		string format = strings::vformat("File: %s -> Line: %d -> Snippet %s",
											d.full_path.c_str(),
											d.line_number,
											d.snipshot.c_str());
		format += "\n"; // making sure line is finished
		fprintf(fp, "%s", format.c_str());
	}
	fclose(fp);	
}

bool helper::is_ext_ignored(const string& fname)
{
	static list<string> ignored = 
	{
	"exe", "css", "doc", "docx" "xls", "xlsx",
	"ppt", "pptx", "odt", "pdf", "ods",
	"mp3", "mp4", "jpg", "jpeg", "png",
	"avi", "wav", "zip", "7z", "rar", "tar.gz",
	"bin", "iso", "mdb", "mdbx", ".tar", "com",
	"msi", "ico", "tif", "tiff", "mkv", "mov",
	"mpg", "mpeg", "swf", "wmv"
	};
	
	/* get file extention */
	string ext = fname.substr(fname.find_last_of(".") + 1);
	if (ext.empty())
		return false;
	
	for (string ignore : ignored)
	{
		if (ext == ignore)
			return true;
	}

	return false;
}
void helper::print_result(const vector<finder::sen_data_t>& results)
{
	for(const finder::sen_data_t& result : results)
	{
		printf("file %s ", result.full_path.c_str());
		cprintf(color::blue, "line ");
		printf("%d: ", result.line_number);
		cprintf(color::green, "%s\n", result.snipshot.c_str());
	}
}

string helper::combine_patterns(const list<string>& patterns) 
{
	string cp;
    for (const string& pattern : patterns) 
	{
        if (!cp.empty()) 
		{
            cp += "|";
        }
        cp += "(" + pattern + ")";
    }
	return cp;
}

vector<finder::sen_data_t> finder::find_sensitive_data(const string& full_path,
					const string& text, 
					const list<string>& patterns) 
{
   	// Combine patterns into a single regex
    std::string combined_pattern = helper::combine_patterns(patterns);

    std::regex pattern_regex(combined_pattern);
    std::istringstream stream(text);
    std::string line;
    int line_number = 0;
    
	vector<sen_data_t> results;
    while (std::getline(stream, line)) 
	{
        line_number++;
        std::smatch matches;

        // Search for the pattern in the current line
        if (std::regex_search(line, matches, pattern_regex)) 
		{
			// sub_match<string::const_iterator>& match
            for (const auto& match : matches)
			{
				std::string matched_string(match.first, match.second);
				sen_data_t data;

                size_t match_index = match.first - line.begin();
                // Print line number, matched pattern, and about 15 characters after it
				// Calculate the number of characters remaining in the line after the match
				size_t remaining_length = match_index < line.length() ? line.length() - match_index : 0;

				// Calculate the length to print
				size_t length = std::min(remaining_length, (size_t)(match.length() + 15));
            	
				if (length == 0)
					break;;

				string snippet = line.substr(match_index, length);
				data.line_number = line_number;
				data.snipshot = snippet;
				data.full_path = full_path;
				results.push_back(data);
            }
        }
    }
	return results;
}

bool helper::is_digit(const string& str) 
{
    char* end;
    errno = 0; // Reset errno before the call
    long val = std::strtol(str.c_str(), &end, 10);

    // Check for conversion errors (no digits found, or not all string was digits)
    if (end == str.c_str() || *end != '\0' || errno == ERANGE)
        return false;

    return true;
}

string get_arg_value(parser *p, const char *arg)
{
	string value = p->get_value(arg);
	if (value.empty())
	{
		string format = strings::vformat("Fatal Error! %s has been sent without value",
			arg);
		cperror((char*)format.c_str());
		exit(EXIT_FAILURE);
	}
	else
	{
		if(!helper::is_digit(value))
		{
			if(value.compare(0,1, "-") == 0)
			{
				string format = strings::vformat("Fatal Error! %s has been sent without value",
					arg);
				cperror((char*)format.c_str());
				exit(EXIT_FAILURE);				
			}
		}

	}
		return value;
}

string get_exec_directory()
{
	string exec_dir;

#ifdef _WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	exec_dir = buffer;
	exec_dir = exec_dir.substr(0, exec_dir.find_last_of("\\/"));
#else
	char buffer[1024];
	int len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	buffer[len] = '\0';
	exec_dir = buffer;
	exec_dir = exec_dir.substr(0, exec_dir.find_last_of("/"));
#endif

	return exec_dir;
}
list<string> parse_words_list(string wordlist)
{
	list<string> l;
	stringstream words(wordlist);
	string token;
	while (std::getline(words, token, '$'))
		l.push_back(token);

	return l;
}
list<string> get_words_from_file(string path)
{
	FILE* fp = fopen(path.c_str(), "r");
	if (fp == NULL)
	{
		cperror((char *)"%s can't be found", path.c_str());
		exit(EXIT_FAILURE);
	}
	list<string> wordlist;
	string line;
	while (strings::getline(fp, line))
		wordlist.push_back(line);

	fclose(fp);
	return wordlist;
}

list<string> get_default_word_list()
{
	list<string> wordlist;
	wordlist.push_front("pass=");
	wordlist.push_front("pass:");
	wordlist.push_front("password=");
	wordlist.push_front("password =");
	wordlist.push_front("password:");
	wordlist.push_front("password\"=\"");
	wordlist.push_front("pwd:");
	wordlist.push_front("pwd=");
	wordlist.push_front("pwd =");
	wordlist.push_front("user=");
	wordlist.push_front("user =");
	wordlist.push_front("user:");
	wordlist.push_front("username:");
	wordlist.push_front("username=");
	wordlist.push_front("username =");
	wordlist.push_front("username\"=\"");
	wordlist.push_front("<username>");
	wordlist.push_front("<password>");
	wordlist.push_front("adm-");
	wordlist.push_front("secret ");
	wordlist.push_front("secret=");
	wordlist.push_front("secret:");
	wordlist.push_front("secret =");
	wordlist.push_front("passwd:");
	wordlist.push_front("passwd=");
	wordlist.push_front("passwd =");

	return wordlist;
}
void helper::print_help()
{
	/*asci art for "FIF(Find InFile)" */
	printf(" ___  _  ___    __ ___  _         _   _        ___  _  _          __  \n| __>| || __>  / /| __><_>._ _  _| | | |._ _  | __><_>| | ___  ___\\ \  \n| _> | || _>  | | | _> | || ' |/ . | | || ' | | _> | || |/ ._><_-< | |\n|_|  |_||_|   | | |_|  |_||_|_|\___| |_||_|_| |_|_| |_|  |_||_|\___./__/ | |\n               \\_\\                                                /_/\n");
	printf("\nfif (Find In Files) is a tool to find sensitive data in files\n"
		"The tool ignore audio/video/office file types and step over by default\n"
		"common directories such: visual studio and nmap ..\n\n"
		"Usage: fif [module] [options...]\n"
		"\nModuls:\n"
		"  local	find sensitive data in the LOCAL files\n"
		"  smb	find sensitive data in SMB share drive\n"
		"\nDebug:\n"
		"  -d	print debug vars\n"
		"  -e	show errors\n"
		"  -v	verbose mode\n"
		"\nOptions:\n"
		"  -w	words list (separated by $)\n"
		"  -W	words list from file\n"
		"  -o	output to a file\n"
		"  -l	limit file size (in KB, default: 2M)\n"
		"  -h	print this help\n"
		"\nUsage Example:\n"
		"  fif local [options ...]\n"
		"  fif smb [options ...]\n"
		"  fif local -w \"word1$word2$word3\" [options ...]\n"
	);
}