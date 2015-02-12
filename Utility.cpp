#include "Utility.h"
#include "String.h"
#include <istream>
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

String title_read(istream &is)
{
    String title;
    getline(is, title);
    title = parse_title(title);
    if (title.size() == 0)
    {
        throw Error("Could not read a title!");
    }
    return title;
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
    if (title.size() > 0 && isspace(title[title.size() - 1]))
    {
        title.remove(title.size() - 1, 1);
    }
    return title;
}