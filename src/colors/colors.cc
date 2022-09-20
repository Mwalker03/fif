#include "colors.h"

#ifdef WIN32
	#include <windows.h>
#endif

/*
	red = 31, blue = 34, green = 32,
	yellow = 33, mangeta = 35, cyan = 36
*/

int ansi_color(color c)
{
	switch (c)
	{
	case color::red:		return 31;
	case color::blue:		return 34;
	case color::green:		return 32;
	case color::yellow:		return 33;
	case color::mangeta:	return 35;
	case color::cyan:		return 36;
	default:
		return 0;
	}
}
#ifdef WIN32
int windows_color(color c)
{
	switch (c)
	{
	case color::red:		return 12;
	case color::blue:		return 9;
	case color::green:		return 10;
	case color::yellow:		return 14;
	case color::mangeta:	return 13;
	case color::cyan:		return 11;
	default:
		return 7;
	}
}

HANDLE set_console_color(color c)
{
	HANDLE hConsole;
	//hConsole = (HANDLE *)malloc(1);
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	int color = windows_color(c);
	SetConsoleTextAttribute(hConsole, (WORD)color);
	return hConsole;
}
int cprintf(color c, const char* format, ...)
{
	va_list arg;
	int done;
	va_start(arg, format);
	HANDLE hConsole = set_console_color(c);
	done = vfprintf(stdout, format, arg);
	SetConsoleTextAttribute(hConsole, (WORD)7);
	va_end(arg);

	return done;
}
void print_windows_colors()
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < 25; i++)
	{
		SetConsoleTextAttribute(hConsole, (WORD)i);
		printf("color %d = %s\n", i, "maor");
	}
}

int cperror(char* format, ...)
{
	va_list arg;
	int ok;
	va_start(arg, format);
	HANDLE hConsole = set_console_color(color::red);
	ok = vfprintf(stderr, format, arg);
	SetConsoleTextAttribute(hConsole, (WORD)7);
	va_end(arg);
	return ok;
}

#endif

#ifdef __linux__ 
int cprintf(color c, const char* format, ...)
{
	va_list arg;
	int done;
	va_start(arg, format);
	char colorname[10] = {0}; //\033[0;31m
	int ac = ansi_color(c);
	sprintf(colorname, "\033[1;%dm", ac);
	printf("%s", colorname);
	done = vfprintf(stdout, format, arg);
	printf("%s", colorname);
	printf("\033[0m");
	va_end(arg);

	return done;
}
int cperror(char* format, ...)
{
	va_list arg;
	int done;
	va_start(arg, format);
	char colorname[10] = {0}; //\033[0;31m
	std::string output; //avoid malloc

	int ac = ansi_color(color::red);
	sprintf(colorname, "\033[1;%dm", ac);
	
	output = colorname;
	output.append(format);
	output.append("\033[0m");

	//printf("%s", colorname);
	done = vfprintf(stderr, output.c_str(), arg);
	//printf("%s", colorname);
	printf("\033[0m");
	va_end(arg);

	return done;
}
#endif