#pragma once

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#include <string>
#include <vector>
#include <string.h>

using namespace std;

class strings
{
public:

    static const std::string vformat(const char* const zcFormat, ...) 
    {
        // initialize use of the variable argument array
        va_list vaArgs;
        va_start(vaArgs, zcFormat);

        // reliably acquire the size
        // from a copy of the variable argument array
        // and a functionally reliable call to mock the formatting
        va_list vaArgsCopy;
        va_copy(vaArgsCopy, vaArgs);
        const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
        va_end(vaArgsCopy);

        // return a formatted string without risking memory mismanagement
        // and without assuming any compiler or platform specific behavior
        std::vector<char> zc(iLen + 1);
        std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
        va_end(vaArgs);
        return std::string(zc.data(), iLen);
    }

    static char* getline(FILE* stream, string &line)
    {
        char buf[BUFSIZ];
        char* res = NULL;
        line.erase(); // init the string
        res = fgets(buf, BUFSIZ, stream);
        // remove the new line trail
        buf[strcspn(buf, "\n")] = '\0';
        line.append(buf);
        return res;
    }
    static size_t getline(char** lineptr, size_t* n, FILE* stream)
    {
        int c;
        size_t len = 0;
        size_t linesize = 0;
        int terminator = '\n';
        const size_t BLOCKSIZE = 255;
        if (lineptr == NULL || stream == NULL || n == NULL)
        {
            errno = EINVAL;
            return -1;
        }
        linesize = BLOCKSIZE;
        *lineptr = (char*)malloc(BLOCKSIZE);
        while ((c = fgetc(stream)) != EOF && c != terminator)
        {
            if ((len + 1) == linesize)
            {
                linesize += BLOCKSIZE;
                *lineptr = (char*)realloc(*lineptr, linesize);
            }
            (*lineptr)[len++] = c;
        }
        if (len == 0 && c != terminator) // check for blank lines
        {
            errno = EINVAL;
            free(*lineptr);
            *lineptr = NULL;
            return -1;
        }
        (*lineptr)[len] = 0; // null-terminate the string

        return len;
    }
    static string to_lower(string str)
    {
        for (char& ch : str)
        {
            ch = tolower(ch);
        }
        return str;
    }
    static string trim(string str) 
    {
        // Remove spaces from the beginning of the string
        int i = 0;
        while (i < str.length() && str[i] == ' ') 
        {
            ++i;
        }
        str.erase(0, i);

        // Remove '\n' and '\t' characters from the end of the string
        i = str.length() - 1;
        while (i >= 0 && (str[i] == '\n' || str[i] == '\t' || str[i] == ' ')) 
        {
            --i;
        }
        str.erase(i + 1);

        return str;
    }
    static bool contains(const string& haystack, const string& needle) 
    {
        return haystack.find(needle) != std::string::npos;
    }
};