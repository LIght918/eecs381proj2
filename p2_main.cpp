#include <iostream>
#include <fstream>
#include "String.h"
#include "Ordered_list.h"
#include "Record.h"
#include "Collection.h"
#include "Utility.h"
#include "p2_globals.h"

using namespace std;

bool record_id_comp(const Record *lhs, const Record *rhs) const { return lhs->get_ID() < rhs->get_ID(); }

void throw_unrecognized_command();

Record* read_title_get_record(Ordered_list<*Record>& library_title);
Ordered_list<*Record>::Iterator read_title_get_iter(Ordered_list<*Record>& library_title);

Record* read_id_get_record(Ordered_list<*Record, record_id_comp>& library_id);
Ordered_list<*Record, record_id_comp>::Iterator read_id_get_iter(Ordered_list<*Record, record_id_comp>& library_id);

Collection* read_name_get_collection(Ordered_list<*Collection>& catalog);
Ordered_list<*Collection>::Iterator read_name_get_iter(Ordered_list<*Collection>& catalog);

void clear_libraries(Ordered_list<*Record> library_title, Ordered_list<*Record, record_id_comp> library_id);
void clear_catalog(Ordered_list<*Collection> catalog);

bool check_collection_not_empty(Collection *collection);
bool check_record_in_collection(Collection *collection, Record *record);

void print_record(Record* record, std::ostream& os);
void print_collection(Collection* collection, std::ostream& os);

int main()
{
    Ordered_list<*Collection> catalog;
    Ordered_list<*Record> library_title;
    Ordered_list<*Record, record_id_comp> library_id;
    while (true)
    {
        try
        {
            char action, object;
            cout << "\nEnter command: ";
            if (!(cin >> action >> object))
            {
                throw_unrecognized_command();
            }
            switch (action)
            {
                case 'f': /* find (records only) */
                {
                    switch (object)
                    {
                        case 'r': /* find record */
                        {
                            auto record_ptr = read_title_get_record(library_title);
                            cout << *record_ptr;
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'p': /* print */
                {
                    switch (object)
                    {
                        case 'r': /* print record */
                        {
                            Record *record_ptr = read_id_get_record(library_id);
                            cout << *record_ptr;
                            break;
                        }
                        case 'c': /* print collection */
                        {
                            Collection *collection_ptr = read_name_get_collection(catalog);
                            cout << *collection_ptr;
                            break;
                        }
                        case 'L': /* print library */
                        {
                            if (library_title.empty())
                            {
                                cout << "Library is empty";
                            }
                            else
                            {
                                cout << "Library contains " << library_title.size() << " records:";
                                apply_arg(library_title.begin(), library_title.end(), print_record, cout);
                            }
                            break;
                        }
                        case 'C': /* print catalog */
                        {
                            if (catalog.empty())
                            {
                                cout << "Catalog is empty";
                            }
                            else
                            {
                                cout << "Catalog contains " << catalog.size() << " collections:";
                                apply_arg(catalog.begin(), catalog.end(), print_collection, cout);
                            }
                            break;
                        }
                        case 'a': /* print memory allocations */
                        {
                            cout << "Memory allocations:\n";
                            cout << "Records: " << library_title.size() << "\n";
                            cout << "Collections: " << catalog.size() << "\n";
                            cout << "Lists: " << g_Ordered_list_count << "\n";
                            cout << "List Nodes: " << g_Ordered_list_Node_count << "\n";
                            cout << "Strings: " << String::get_number() << " with " << String::get_total_allocation() << " bytes total";
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'm': /* modify (rating only) */
                {
                    switch (object)
                    {
                        case 'r': /* modify rating of a record */
                        {
                            Record *record_ptr = read_id_get_record(library_id);
                            int rating = integer_read();
                            record_ptr->set_rating(rating);
                            cout << "Rating for record " <<  " changed to " << rating;
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'a': /* add */
                {
                    switch (object)
                    {
                        case 'r': /* add record */
                        {
                            String medium, title;
                            cin >> medium;
                            title = title_read();
                            if (library_title.find(&Record(title)) != nullptr)
                            {
                                throw Error("Library already has a record with this title!");
                            }
                            Record *record = new Record(medium, title);
                            library_id.insert(record);
                            library_title.insert(record);
                            cout << "Record " << record->get_ID() << " added";
                            break;
                        }
                        case 'c': /* add collection */
                        {
                            String name;
                            cin >> name;
                            if (catalog.find(&Collection(name)) != nullptr)
                            {
                                throw Error("Catalog already has a collection with this name!");
                            }
                            catalog.insert(new Collection(name));
                            cout << "Collection " << name << " added";
                            break;
                        }
                        case 'm': /* add record to collection */
                        {
                            Collection *collection_ptr = read_name_get_collection(catalog);
                            Record *record_ptr = read_title_get_record(library_title);
                            collection_ptr->add_member(record_ptr);
                            cout << "Member " << record_ptr->get_ID() << " " << record_ptr->get_title() << " added";
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'd': /* delete */
                {
                    switch (object)
                    {
                        case 'r': /* delete record */
                        {
                            auto record_iter = read_title_get_iter(library_title);
                            if (apply_if_arg(catalog.begin(), catalog.end(), check_record_in_collection, *record_iter))
                            {
                                throw Error("Cannot delete a record that is a member of a collection!");
                            }
                            Record *record_ptr = *record_iter;
                            library_title.erase(record_iter);
                            library_id.erase(library_id.find(record_ptr));
                            cout << "Record " << record_ptr << " deleted";
                            delete record_ptr;
                            break;
                        }
                        case 'c': /* delete collection */
                        {
                            auto collection_iter = read_name_get_collection(catalog);
                            Collection *collection_ptr = *collection_iter;
                            catalog.erase(collection_iter);
                            cout << "Collection " << collection_ptr->get_name() << " deleted";
                            delete collection_ptr;
                            break;
                        }
                        case 'm': /* delete record from collection */
                        {
                            Collection *collection_ptr = read_name_get_collection(catalog);
                            Record *record_ptr = read_title_get_record(library_title);
                            collection_ptr->remove_member(record_ptr);
                            cout << "Member " <<record_ptr->get_ID() << " " << record_ptr->get_title() << " deleted";
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'c': /* clear */
                {
                    switch (object)
                    {
                        case 'L': /* clear library */
                        {
                            if (apply_if(catalog.begin(), catalog.end(), check_collection_not_empty))
                            {
                                throw Error("Cannot clear all records unless all collections are empty!");
                            }
                            Record::reset_ID_counter();
                            clear_libraries(library_title, library_id);
                            cout << "All records deleted";
                            break;
                        }
                        case 'C': /* clear catalog */
                        {
                            clear_catalog(catalog);
                            cout << "All collections deleted";
                            break;
                        }
                        case 'A': /* clear all */
                        {
                            Record::reset_ID_counter();
                            clear_libraries(library_title, library_id);
                            clear_catalog(catalog);
                            cout << "All data deleted";
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 's': /* save */
                {
                    switch (object)
                    {
                        case 'A': /* save all */
                        {
                            String filename;
                            cin >> filename;
                            ofstream file(filename.c_str());
                            if (!file)
                            {
                                throw_file_error();
                            }
                            file << library_title.size() << "\n";
                            for (auto&& record : library_title)
                            {
                                record->save(file);
                            }
                            file << catalog.size() << "\n";
                            for (auto catalog_iter = catalog.begin(); catalog_iter != catalog.end(); catalog_iter++)
                            {
                                catalog_iter->save(file);
                                catalog_iter++;
                            }
                            file.close();
                            cout << "Data saved";
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'r': /* restore */
                {
                    switch (object)
                    {
                        case 'A': /* restore all */
                        {
                            String filename;
                            cin >> filename;
                            ifstream file(filename.c_str());
                            if (!file)
                            {
                                throw_file_error();
                            }
                            int num;
                            file >> num;
                            Ordered_list<*Collection> new_catalog;
                            Ordered_list<*Record> new_library_title;
                            Ordered_list<*Record, record_id_comp> new_library_id;
                            try
                            {
                                Record::save_ID_counter();
                                Record::reset_ID_counter();
                                while (num > 0)
                                {
                                    Record *record_ptr = new Record(file);
                                    new_library_title.insert(record_ptr);
                                    new_library_id.insert(record_ptr);
                                    num--;
                                }
                                file >> num;
                                while (num > 0)
                                {
                                    Collection *collection_ptr = new Collection(file);
                                    new_catalog.insert(collection_ptr);
                                    num--;
                                }
                                clear_libraries(library_title, library_id);
                                clear_catalog(catalog);
                                library_title = new_library_title;
                                library_id = new_library_id;
                                catalog = new_catalog;
                                cout << "Data loaded";
                            }
                            catch (Error& e)
                            {
                                Record::restore_ID_counter();
                                throw e;
                            }
                            break;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                case 'q': /* quit */
                {
                    switch (object)
                    {
                        case 'q': /* quit */
                        {
                            cout << "Done";
                            return 0;
                        }
                        default:
                        {
                            throw_unrecognized_command();
                            break;
                        }
                    }
                    break;
                }
                default:
                {
                    throw_unrecognized_command();
                    break;
                }
            }
        } catch (Error& e)
        {
            cout << e.msg << "\n";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        cout << "\n";
    }
}

void throw_unrecognized_command()
{
    throw Error("Unrecognized command!");
}

Record* read_title_get_record(Ordered_list<*Record>& library_title)
{
    return *read_title_get_iter(library_title);
}

Ordered_list<*Record>::Iterator read_title_get_iter(Ordered_list<*Record>& library_title)
{
    String title = title_read();
    auto record_iter = library_title.find(&Record(title));
    if (record_iter == nullptr)
    {
        throw Error("No record with that title!");
    }
    return record_iter;
}

Record* read_id_get_record(Ordered_list<*Record, record_id_comp>& library_id)
{
    return *read_id_get_iter(library_id);
}

Ordered_list<*Record, record_id_comp>::Iterator read_id_get_iter(Ordered_list<*Record, record_id_comp>& library_id)
{
    int id = integer_read();
    auto record_iter = library_id.find(&Record(id));
    if (record_iter == nullptr)
    {
        throw Error("No record with that ID!");
    }
    return record_iter;
}

Collection* read_name_get_collection(Ordered_list<*Collection>& catalog)
{
    return *read_name_get_iter(catalog);
}

Ordered_list<*Collection>::Iterator read_name_get_iter(Ordered_list<*Collection>& catalog)
{
    String name;
    cin >> name;
    auto collection_iter = catalog.find(name);
    if (collection_iter == nullptr)
    {
        throw Error("No collection with that name!");
    }
    return collection_iter;
}

void clear_libraries(Ordered_list<*Record> library_title, Ordered_list<*Record, record_id_comp> library_id)
{
    auto title_iter = library_title.begin();
    while (title_iter != nullptr)
    {
        delete *title_iter;
        title_iter++;
    }
    auto id_iter = library_id.begin();
    while (id_iter != nullptr)
    {
        delete *id_iter;
        id_iter++;
    }
    library_title.clear();
    library_id.clear();
}

void clear_catalog(Ordered_list<*Collection> catalog)
{
    auto catalog_iter = catalog.begin();
    while (catalog_iter != nullptr)
    {
        delete *catalog_iter;
        catalog_iter++;
    }
    catalog.clear();
}

bool check_collection_not_empty(Collection *collection)
{
    return !(collection->empty());
}

bool check_record_in_collection(Collection *collection, Record *record)
{
    return collection->is_member_present(record);
}

void print_record(Record* record, std::ostream& os)
{
    os << "\n" << *record;
}

void print_collection(Collection* collection, std::ostream& os)
{
    os << "\n" << *collection;
}