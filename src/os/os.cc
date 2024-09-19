#include "os.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <thread>
#include <map>
#include <future>
#include <queue>

#ifdef _WIN32
	#include <direct.h>
	#include <io.h>
	#include "dirent.h"
	#define chdir _chdir
	#define getcwd _getcwd
#else
	#include <unistd.h>
	#include <limits.h>
	#define _MAX_PATH PATH_MAX
#endif

#include <sys/stat.h>


string os::get_fullpath(string currdir, string fname) 
{
    char fullpath[_MAX_PATH] = { 0 };
    int ok = chdir(currdir.c_str());
    if (ok == -1)
        return fullpath;

#ifdef _WIN32
    if (_fullpath(fullpath, fname.c_str(), _MAX_PATH) != NULL) {
        return fullpath;
    }
#else
    if (realpath(fname.c_str(), fullpath) != NULL) {
        return fullpath;
    }
#endif

    return fullpath;
}

size_t os::get_file_size(string fpath)
{
	struct stat st;
	stat(fpath.c_str(), &st);
	long fsize = st.st_size;
	return fsize;
}
string os::get_file_ext(string fname)
{
	return fname.substr(fname.find_last_of(".") + 1);
}

bool is_ignored(char* filename, list<string> ignored)
{
	string fname(filename);
	string ext = os::get_file_ext(fname);
	if (ext.empty())
		return false;
	for (string ignore : ignored)
	{
		if (ext == ignore)
			return true;
	}
	return false;
}

list<fileentry*> os::get_directory(const string& dpath, 
									int size_limit=0,
									list<string> ignore={0},
									bool show_errors=false)
{
	DIR* dir;
	dirent* entry;
	list<fileentry*> entries;

	if ((dir = opendir(dpath.c_str())) != NULL)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			if (strcmp(entry->d_name, ".") == 0 || 
				strcmp(entry->d_name, "..") == 0)
				continue;

			/* 
				to avoid memory leak, we first declare
				the struct on the stack, then, 
				return a pointer.
			 */
			fileentry fe;
			string filefullpath = get_fullpath(dpath, entry->d_name);
			if (entry->d_type == DT_REG)
			{
				if (ignore.size() > 0)
				{
					// filter extentions
					if (is_ignored(entry->d_name, ignore))
						continue;
				}
				if (size_limit > 0)
				{
					// limit file size
					long filesize = get_file_size(filefullpath);
					if (filesize > size_limit)
						continue;
					else
						fe.size = filesize;
				}
				fe.isfile = true;
			}
			else if (entry->d_type == DT_DIR)
				fe.isfile = false;
			else
			{
				/* entry is not a file or directory
					so its unwanted */
				continue;
			}
			fe.name = entry->d_name;
			fe.path = filefullpath;
			
			fileentry* pfe = new fileentry;
			pfe->isfile = fe.isfile;
			pfe->name = fe.name;
			pfe->path = fe.path;
			pfe->size = fe.size;
			
			entries.push_back(pfe);
		}
		closedir(dir);
	}
	else
	{
		if (show_errors)
		{
			printf("opendir %s error (%d: %s)\n",
				dpath.c_str(), errno, strerror(errno));
		}
	}
	return entries;
}
list<fileentry> os::files;
list<fileentry> os::get_directory_r(const string& root,
								int size_limit=0,
								list<string> ignore={0},
								bool show_errors=false)
{
	list<fileentry*> entries = os::get_directory(root, size_limit,
												ignore, show_errors);
	for (fileentry* entry : entries)
	{
		os::files.push_back(*entry);
		if (!entry->isfile)
			os::get_directory_r(entry->path, size_limit,
						ignore, show_errors);
		delete entry;
	}
	return os::files;
}

