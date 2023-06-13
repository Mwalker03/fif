#pragma once
#ifdef __GNUC__
#define LINUX
#else
#define WINDOWS
#include "dirent.h"
#endif

#include <string>
#include <list>
#include <vector>

using namespace std;

typedef struct _fileentry
{
	string	name;
	string	path;
	time_t	ctime;
	time_t	atime;
	size_t	size;
	bool	isfile;
}fileentry;


class os
{
public:
	static size_t get_file_size(string fpath);
	static string get_file_ext(string fname);
	static list<fileentry> get_directory_r(const string& root, 
									int size_limit,
									list<string> ignore,
									bool show_errors); // Read directory include Sub-Dirs
	static list<fileentry*> get_directory(const string& dpath,
										int size_limit,
										list<string> ignore,
										bool show_errors); 
	static string get_fullpath(string currdir, string fame);

private:
	static list<fileentry> files;
};