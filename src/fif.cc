#include "fif.h"

#ifdef WIN32_LEAN_AND_MEAN
#include <direct.h>
#else
#include <dirent.h>
#include <limits.h>
#include  <unistd.h>
#include <sys/stat.h>
#endif

extern configuration_manager_t config;

void write_to_file(string str, string filepath)
{
	FILE* fp = fopen(filepath.c_str(), "a+");
	if (fp == NULL)
	{
		if (config.show_errors)
			cperror((char *)"Failed to open %s for writing (%d: %s)\n",
				filepath.c_str(), errno, strerror(errno));
	}
	else
	{
		str += "\n"; // making sure line is finished
		fprintf(fp, str.c_str());
		fclose(fp);
	}
}

void find_sensitive_v2(string fname, list<string> pattrens)
{
	FILE* fp = NULL;
	if ((fp = fopen(fname.c_str(), "r")) == NULL)
	{
		if (config.show_errors)
			cperror((char *)"Failed to open %s for reading (%d: %s)\n",
				fname.c_str(), errno, strerror(errno));

		return;
	}

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
					printf("file %s ", fname.c_str());
					cprintf(color::blue, "line ");
					printf("%d: ", line_counter);
					cprintf(color::green, "%s\n", sub.c_str());
					if (config.to_file)
					{
						string format = strings::vformat("file: %s line %d %s", 
															fname.c_str(),
															line_counter, 
															sub.c_str());
						write_to_file(format, config.output_file_path);
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


void scan_r(string root, list<string> pattrens)
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

	files = os::get_directory(root, config.file_size, 
								ignored, config.show_errors);

	for (fileentry *entry : files)
	{
		if (config.verbose)
			cprintf(color::mangeta,
				"entry.path: %s\n", entry->path.c_str());

		if (entry->isfile)
			find_sensitive_v2(entry->path, pattrens);
		else
			scan_r(entry->path, pattrens);

		delete entry;
	}
}