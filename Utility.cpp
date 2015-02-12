#include "Utility.h"
#include "String.h"
#include <sstream>
#include <iostream>
#include <cctype>

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
        title(parse_title(title));
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
    String title(title_string);
    for (int i = 0; i < title.size(); i++)
    {
        if (!isspace(title[i]))
        {
            title.remove(0, i);
            break;
        }
    }
    for (int i = 0; i < title.size(); i++)
    {
        if (isspace(title[i]))
        {
            int j;
            for (j = i + 1; j < title.size() && isspace(title[j]); j++);
            if (j != i + 1)
            {
                title.remove(i + 1, j - (i + 1));
            }
        }
    }
    if (isspace(title[title.size() - 1]))
    {
        title.remove(title.size() - 1, 1);
    }
    return title;
}