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
        title = parse_title(title);
        if (title.size() == 0)
        {
            throw String_exception("");
        }
        return title;
    } catch (String_exception& e)
    {
        cerr << e.msg << "\n";
        throw Error("Could not read a title!");
    }
}

String parse_title(String& title_string)
{
    istringstream is(title_string.c_str());
    String title, substring;
    while(is >> substring)
    {
        title += substring;
        title += " ";
    }
    if (title.size() == 0)
    {
        return String();
    }
    title.remove(title.size() - 1, 1);
    cerr << "printing parsed title" << endl;
    cerr << title << endl;
    return title;
}