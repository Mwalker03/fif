#pragma once

#include <string>
#include <string.h>
#include <inttypes.h>
#include <vector>
#include <list>

#include <time.h>
#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include <smb2/libsmb2-raw.h>
#include <cinttypes>

#ifdef __AROS__
#include "asprintf.h"
#endif

#include "../argsparser/argsparser.h"
#include "../strings.h"

using namespace std;

typedef struct _smb_config
{
    string username;
    string password;
    string domain;
    string server;
    string start_point;
	list<string> words_list;
    bool showHelp = false;
}smb_config_t;

typedef struct _smb_entry
{
    string name;
    string path;
    string full_path;
    size_t size;
    bool isfile;
}smb_entry;

class smb
{
private:
    static smb_config_t cnf;
    static struct smb2_context *smb2;
    static struct smb2_url *url;

    static string get_arg_value(parser *p, const char *key);
    static bool init();
    static vector<smb_entry> get_directory(string dir_name);
    static string get_file_content(smb_entry& ent);

public:
    static smb_config_t parse_smb_args(parser *p);
    static void scan_r(string root, const smb_config_t& smb_cnf);
    static void read_files(string un, string ps);
    static void free();

};
