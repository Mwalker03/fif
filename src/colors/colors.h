#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string>

enum color {
	red, blue, green, 
	yellow, mangeta, cyan
};

void print_windows_colors();

int ansi_color(color c);
int windows_color(color c);
int cprintf(color c, const char* format, ...);
//int cprintf(FILE* const_stream, color c, char* format, ...);
int cperror(char* format, ...);
