#ifndef COLLECTION_H
#define COLLECTION_H

#include <fstream>
#include <ostream>
#include "p2_globals.h"
#include "Utility.h"
#include "Record.h"
#include "Ordered_list.h"
#include "String.h"

/* Construct a Collection from an input file stream in save format, using the record list,
    restoring all the Record information.
    Record list is needed to resolve references to record members.
    No check made for whether the Collection already exists or not.
    Throw Error exception if invalid data discovered in file.
    String data input is read directly into the member variable. */
Collection::Collection(std::ifstream& is, const Ordered_list<Record*, Less_than_ptr<Record*>()>& library)
{

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
    return elements.find(record_ptr) != nullptr;
}
// Remove the specified Record, throw exception if the record was not found.
void Collection::remove_member(Record* record_ptr)
{
    Ordered_list<Record*, Less_than_ptr<Record*>::Iterator it = elements.find(record_ptr);
    if (it == nullptr)
    {
        throw Error("Record is not a member in the collection!");
    }
    elements.erase(it);
}

// Write a Collections's data to a stream in save format, with endl as specified.
void Collection::save(std::ostream& os) const
{

}

// Print the Collection data
std::ostream& operator<< (std::ostream& os, const Collection& collection);

#endif