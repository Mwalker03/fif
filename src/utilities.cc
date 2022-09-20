#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"
#include "colors/colors.h"
#include "strings.h"

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

	return wordlist;
}
void print_help()
{
	printf("\nfif (Find In Files) is a tool to find sensitive data in files\n"
		"The tool ignore audio/video/office file types\n\n"
		"Usage: fif [options...]\n"
		"  -sp	start point"
		"  -d	print debug vars\n"
		"  -e	show errors\n"
		"  -v	verbose mode\n"
		"  -w	words list (separated by $)\n"
		"  -W	words list from file\n"
		"  -o	output to a file\n"
		"  -l	limit file size (in KB, default: 2M)\n"
		"  -h	print this help\n"
		"\nUsage Example:\n"
		"  fif 		# start from current location\n"
		"  fif -sp /	# start from root\n"
		"  fif [location]\n"
		"  fif -w \"word1$word2$word3\" -sp c:\\users\\[username]");
}