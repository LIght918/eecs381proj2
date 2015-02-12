#include "Utility.h"
#include "String.h"
#include <sstream>
#include <iostream>

using namespace std;

void throw_file_error()
{
    throw Error("Invalid data found in file!");
}

int integer_read()
{
    int integer;
    if (!(cin >> integer))
    {
        throw Error("Could not read an integer value!");
    }
    return integer;
}

String title_read()
{
    try
    {
        String title;
        getline(cin, title);
        parse_title(title);
        if (title.size() == 0)
        {
            throw String_exception("");
        }
        return title;
    } catch (String_exception& e)
    {
        throw Error("Could not read a title!");
    }
}

String parse_title(String& title_string)
{
    istringstream is(title_string.c_str());
    String title;
    String substring;
    while(is >> substring)
    {
        title += substring;
    }
    return title;
}