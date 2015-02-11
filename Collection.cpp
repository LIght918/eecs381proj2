#include <fstream>
#include <iostream>
#include "p2_globals.h"
#include "Utility.h"
#include "Record.h"
#include "Collection.h"
#include "Ordered_list.h"
#include "String.h"

/* Construct a Collection from an input file stream in save format, using the record list,
    restoring all the Record information.
    Record list is needed to resolve references to record members.
    No check made for whether the Collection already exists or not.
    Throw Error exception if invalid data discovered in file.
    String data input is read directly into the member variable. */
Collection::Collection(std::ifstream& is, const Ordered_list<Record*, Less_than_ptr<Record*>>& library)
{
    int num;
    if (!(is >> name >> num))
    {
        throw_file_error();
    }
    for (int i = 0; i < num; i++)
    {
        try
        {
            String title = title_read();
            Record temp_record(title);
            auto record_it = library.find(&temp_record);
            if (record_it == library.end())
            {
                throw_file_error();
            }
            elements.insert(*record_it);
        } catch (String_exception& e)
        {
            throw_file_error();
        }
    }
}

// Add the Record, throw exception if there is already a Record with the same title.
void Collection::add_member(Record* record_ptr)
{
    if (is_member_present(record_ptr))
    {
        throw Error("Record is already a member in the collection!");
    }
    elements.insert(record_ptr);
}
// Return true if the record is present, false if not.
bool Collection::is_member_present(Record* record_ptr) const
{
    return elements.find(record_ptr) != elements.end();
}
// Remove the specified Record, throw exception if the record was not found.
void Collection::remove_member(Record* record_ptr)
{
    auto it = elements.find(record_ptr);
    if (it == elements.end())
    {
        throw Error("Record is not a member in the collection!");
    }
    elements.erase(it);
}

void print_record_title(Record* record, std::ostream* os)
{
    *os << "\n" << record->get_title();
}

// Write a Collections's data to a stream in save format, with endl as specified.
void Collection::save(std::ostream& os) const
{
    os << name << " " << elements.size();
    apply_arg(elements.begin(), elements.end(), print_record_title, &os);
    os << "\n";
}

// Print the Collection data
std::ostream& operator<< (std::ostream& os, const Collection& collection)
{
    os << "Collection " << collection.name << " contains:";
    if (collection.empty())
    {
        os << " None";
    }
    else
    {
        apply_arg(collection.elements.begin(), collection.elements.end(), print_record_title, &os);
    }
    return os;
}