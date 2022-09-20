#pragma once
#include <string>

using namespace std;

class parser
{
private:
	int				argc = 0;
	const char**	argv;
	int				find_key(const char* key);

public:
	bool	is_valid();
	bool	has_kay(const char* key);
	bool	has_value(const char* key);
	string	get_value(const char* key);
	void	print_args();

	parser(int argc, const char** argv)
	{
		this->argc = argc;
		this->argv = argv;
	}
};
