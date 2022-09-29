#include "argsparser.h"

#include <string.h>

bool parser::is_valid()
{
	if (this->argc > 2)
		return true;
	else
		return false;
}
int parser::find_key(const char* key)
{
	if (this->argc > 1)
	{
		for (size_t i = 0; i < this->argc; i++)
		{
			if (strcmp(this->argv[i], key) == 0)
				return i;
		}
	}
	return -1;
}
bool parser::has_kay(const char* key)
{
	if (this->argc > 1)
	{
		for (size_t i = 0; i < this->argc; i++)
		{
			if (strcmp(this->argv[i], key) == 0)
				return true;
		}
	}
	return false;
}
bool parser::has_value(const char* key)
{
	int place = this->find_key(key);
	if (place > 0)
		return true;
	else
		return false;
}
string parser::get_value(const char* key)
{
	if (!this->is_valid())
		return "";

	for (size_t i = 0; i < this->argc; i++)
	{
		if (strcmp(this->argv[i], key) == 0)
		{
			if (this->argv[i + 1] != NULL)
			{
				string ret(this->argv[i + 1]);
				return ret;
			}
		}
	}
	return "";
}
void parser::print_args()
{
	if (this->argc > 0)
	{
		printf("argc: %d\n", this->argc);
		for (size_t i = 0; i < this->argc; i++)
		{
			printf("arg%d: %s\n", i, this->argv[i]);
		}
	}
}