#ifndef RECORD_H
#define RECORD_H

#include "p2_globals.h"
#include <fstream>
#include <ostream>
#include <ctype.h>
#include "Record.h"
#include "String.h"
#include "Utility.h"

// Create a Record object, giving it a unique ID number by first incrementing
// a static member variable then using its value as the ID number. The rating is set to 0.
Record::Record(const String &medium_, const String &title_)
{
    title = title_;
    medium = medium_;
    ID = ++ID_counter;
}

// Create a Record object suitable for use as a probe containing the supplied
// title. The ID and rating are set to 0, and the medium is an empty String.
Record::Record(const String &title_)
{
    title = title_;
    ID = ++ID_counter;
}

// Create a Record object suitable for use as a probe containing the supplied
// ID number - the static member variable is not modified.
// The rating is set to 0, and the medium and title are empty Strings.
Record::Record(int ID_)
{
    ID = ID_;
}

// Construct a Record object from a file stream in save format.
// Throw Error exception if invalid data discovered in file.
// No check made for whether the Record already exists or not.
// Input string data is read directly into the member variables.
// The record number will be set from the saved data.
// The static member variable used for new ID numbers will be set to the saved
// record ID if the saved record ID is larger than the static member variable value.
Record::Record(std::ifstream &is)
{
    if (!(is >> ID >> medium >> rating))
    {
        throw_file_error();
    }
    if (ID > ID_counter)
    {
        ID_counter = ID;
    }
    getline(is, title);
    int first_char = -1;
    for (int i = 0; i < title.size(); i++)
    {
        if (!isspace(title[i]))
        {
            first_char = i;
            break;
        }
    }
    if (first_char == -1)
    {
        throw_file_error();
    }
    title = title.substring(first_char, title.size() - first_char);
}

// if the rating is not between 1 and 5 inclusive, an exception is thrown
void Record::set_rating(int rating_)
{
    rating = rating_;
}

// Write a Record's data to a stream in save format with final endl.
// The record number is saved.
void Record::save(std::ostream &os) const
{
    os << ID << " " << medium << " " << rating << " " << title << endl;
}

// Print a Record's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating, title, separated by one space.
// If the rating is zero, a 'u' is printed instead of the rating.
std::ostream& operator<< (std::ostream& os, const Record& record)
{
    os << record.ID << ": " << record.medium << " " << record.rating != 0 ? record.rating : 'u' << " " << record.title;
}

#endif