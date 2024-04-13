#include "smb.h"
#include <fcntl.h> // For O_RDONLY and other flags

#include "../strings.h"
#include "../utilities.h"
#include "../configuration.h"

smb_config_t smb::cnf;
struct smb2_context* smb::smb2 = NULL;
struct smb2_url* smb::url = NULL;

void smb::show_help()
{
    printf("%s", "Welcome to smb module:\n"
		"\nOptions:\n"
		"  -u	username\n"
		"  -p	password\n"
		"  -d	domain\n"
		"  -s	servername\\IP\n"
		"  -f	shared folder to start from\n"
		"  -w	words list (separated by $)\n"
		"  -W	words list from file\n"
		"  -o	output to a file\n"
		"  -l	limit file size (in KB, default: 2M)\n"
        "\nUsage Example:\n"
		"  fif smb -u username -p password -s servername -f foldername\n"
		"  fif smb -d domainname -u username -p password -s servername -f foldername -o filename\n"

    );
    //printf("%s", "Usage: fif smb -u username -p password -d domain -s server -f path/to/folder\n");
}
string smb::get_arg_value(parser *p, const char *key)
{
	string value = p->get_value(key);
	if (value.empty())
	{
		string format = strings::vformat("Fatal Error! %s has been sent without value",
			key);
        
        printf("%s", format.c_str());
		exit(EXIT_FAILURE);
    }
    else
        return value;
}

smb_config_t smb::parse_smb_args(parser *p)
{
    smb_config_t config;
    if(p->has_kay("-h"))
    {
        //config.showHelp = true;
        smb::show_help();
        exit(EXIT_SUCCESS);
    }
    if(p->has_kay("-u"))
    {
        config.username = smb::get_arg_value(p, "-u");
        if(config.username.empty())
        {
            perror("You must provied a username\n");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        perror("You must provied a username via the parameter -u\n");
        exit(EXIT_FAILURE);
    }
    if(p->has_kay("-p"))
    {
        config.password = smb::get_arg_value(p, "-p");
        if(config.password.empty())
        {
            perror("You must provied a password\n");
            exit(EXIT_FAILURE);
        }        
    }
    else
    {
        perror("You must provied password via the -p parameter\n");
        exit(EXIT_FAILURE);        
    }
    if(p->has_kay("-d"))
    {
        config.domain = smb::get_arg_value(p, "-d");
    }
    if(p->has_kay("-s"))
    {
        config.server = smb::get_arg_value(p, "-s");
    }
    if(p->has_kay("-f"))
    {
        config.start_point = smb::get_arg_value(p, "-f");
        if(config.start_point.empty())
        {
            perror("You must provied a folder\n");
            exit(EXIT_FAILURE);
        }        
    }
    else
    {
        perror("You must provied a folder via the -f patameter\n");
        exit(EXIT_FAILURE);        
    }
    return config;
};


bool smb::init()
{
    if(smb2 == NULL)
    {
        smb::smb2 = smb2_init_context();
        if (smb2 == NULL) 
        {
            fprintf(stderr, "Failed to init context\n");
            exit(0);
        }        
        string share_url = strings::vformat("smb://%s@%s/%s", 
                                smb::cnf.username.c_str(), smb::cnf.server.c_str(), 
                                smb::cnf.start_point.c_str());
        

        smb::url = smb2_parse_url(smb2, share_url.c_str());
        if (url == NULL) 
        {
            fprintf(stderr, "Failed to parse url: %s\n",
                    smb2_get_error(smb::smb2));
            exit(0);
        }

        smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
        smb2_set_user(smb2, cnf.username.c_str());
        smb2_set_password(smb2, cnf.password.c_str());

        if (smb2_connect_share(smb::smb2, url->server, url->share, url->user) < 0) 
        {
            printf("smb2_connect_share failed. %s\n", smb2_get_error(smb2));
            printf("debug:\nshare_url -> %s\n", share_url.c_str());
            exit(10);
        }
    }

    return true;
}

vector<smb_entry> smb::get_directory(string dir_name)
{
    struct smb2dir *dir;
    struct smb2dirent *ent;

    string full_url = strings::vformat("smb://%s@%s/%s",
                                    smb::cnf.username.c_str(),
                                    smb::cnf.server.c_str(),
                                    dir_name.c_str());

    //printf("full url: %s\n", full_url.c_str());
    smb::url = smb2_parse_url(smb::smb2, full_url.c_str());
    dir = smb2_opendir(smb2, url->path);
	if (dir == NULL) {
		printf("smb2_opendir failed. %s\n", smb2_get_error(smb2));
		exit(10);
	}

    vector<smb_entry> files;
    while ((ent = smb2_readdir(smb2, dir))) 
    {
        if (strcmp(ent->name, ".") == 0 || 
            strcmp(ent->name, "..") == 0)
            continue;

        char *type;
        time_t t;

        t = (time_t)ent->st.smb2_mtime;
        smb_entry file;
        switch (ent->st.smb2_type) 
        {
        case SMB2_TYPE_LINK:
                type = (char *)"LINK";
                break;
        case SMB2_TYPE_FILE:
                type = (char *)"FILE";
                file.isfile = true;
                break;
        case SMB2_TYPE_DIRECTORY:
                type = (char *)"DIRECTORY";
                file.isfile = false;
                break;
        default:
                type = (char *)"unknown";
                break;
        }
        
        file.name = ent->name;
        file.size = ent->st.smb2_size;
        file.path = strings::vformat("%s/%s", dir_name.c_str(), ent->name);
        file.full_path = strings::vformat("%s/%s", full_url.c_str(), ent->name);
        //printf("file path: %s\n", file.path.c_str());
        files.push_back(file);
    }
    smb2_closedir(smb2, dir);
    return files; 
}

void smb::scan_r(string root, const smb_config_t& smb_cnf)
{
    smb::cnf = smb_cnf;
    if(!smb::init())
    {
        puts("error init smb connection");
        exit(1);
    }
    //printf("get_directory(root): %s\n", root.c_str());
    vector<smb_entry> entries = smb::get_directory(root);
    
    // vector contains all the sensitive data founded 
    for (smb_entry entry : entries)
    {
        if(entry.isfile)
        {
            if(helper::is_ext_ignored(entry.name))
                continue;

            string file_content = smb::get_file_content(entry);
            /*  
                find sensitive in files 
                send the the full file name
                so it will be added to the sen_data_t 
            */
            vector<finder::sen_data_t> results;
            results = finder::find_sensitive_data(entry.full_path,
                                                    file_content,
                                                    global_config.words_list);
            helper::print_result(results);
            if(global_config.to_file)
                helper::log(global_config.output_file_path, results);
        }
        else
        {
            smb::scan_r(entry.path, smb_cnf);
        }
    }
    //smb::free();
}

void smb::free()
{
    if(smb2 != NULL)
    {
        smb2_disconnect_share(smb2);
        smb2_destroy_context(smb2);    
    }
    if(url != NULL)
        smb2_destroy_url(url);
    
}

string smb::get_file_content(smb_entry& ent)
{
    struct smb2fh *fh;

    string content;
    content.resize(ent.size);

    // Open the file
    //printf("ent.full_path: %s\n", ent.full_path.c_str());
    smb::url = smb2_parse_url(smb2, ent.full_path.c_str());
    if(smb::url->path == NULL)
    {
        printf("ent.full_path -> %s", ent.full_path.c_str());
    }
    fh = smb2_open(smb2, smb::url->path, O_RDONLY);
    if (fh == NULL) {
        printf("Failed to open file: %s\n", smb2_get_error(smb2));
        return "";
    }    

    // Read the file's content directly into the string's buffer
    ssize_t read_bytes = smb2_read(smb2, fh, (uint8_t *)&content[0], ent.size);
    if (read_bytes < 0) 
    {
        printf("Error reading file: %s\n", smb2_get_error(smb2));
        smb2_close(smb2, fh);
        content.clear(); // Clear the content on error
        return "";
    }

    // Resize the string to the actual bytes read (in case it was less than expected)
    content.resize(read_bytes);

    // Close the file handle
    smb2_close(smb2, fh);    

    return content;
}
