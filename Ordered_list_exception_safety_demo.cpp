/*
This demo shows how Ordered_list is exception safe. 
We have Thing objects whose constructors are rigged to "blow up" 
when a certain number of them have been constructed - 
it throws an exception from a constructor. In addition, 
Thing objects output messages from constructors and destructors
so we can see who gets created and when they get destroyed. 

These messages allows us to see whether Ordered_list properly 
cleans up if construction of the data items fail.

The basic exception guarantee is that if an operation fails, 
no memory is leaked, and class invariant is maintained.
In this case, it means that any Thing objects created during a failed operation get 
deleted in the process of unwinding the stack, and the Ordered_list remains 
at least functional enough to be destroyed or assigned to.

The strong exception guarantee is that if an operation on 
an existing Ordered_list fails, then the existing list has the same contents 
that it had before the operation was attempted. Ordered_list also has this property, 
as shown in this demo.

For example, in copy assignment of two Ordered_lists, 
if the copy operation fails for one of the Thing objects, then the left-hand list
is left intact, and any Thing objects in the list Nodes that were 
created before the failure get deleted as well.

This demo also shows how using move construction or move assignment on Ordered_lists 
does not involve any copying of Thing objects (and in the case of this container,
no move operations on Thing objects are needed). In general, these operations 
provide the no-throw guarantee - no exceptions should get thrown.
*/

#include "Ordered_list.h"

#include <iostream>

using namespace std;

// First declare the Thing class and related items.

// Things have an ID number used to compare them,
// and also a serial number that is unique for every consructed object.
// A static member variable holds a count-down value, and
// the constructor decrements and throws an exception if the count-down is zero.
// Constructor and destructor messages document when these objects are created/destroyed

struct Thing_exception {
    Thing_exception(const char* msg_) : msg(msg_) {}
    const char* msg;
};

class Thing {
public:
	// regular constructor blows up on countdown and throws an exception
	Thing(int id_) : id(id_)
    {
        count_down--;
        if(count_down == 0) {
            cout << "Attempt to construct Thing " << object_counter + 1 << " with ID " << id << " failed!!!" << endl;
            throw Thing_exception("Boom!");
            }
        // only now increment the object counter and save the serial number
        serial_number = ++object_counter;   // first one is one
        cout << "Thing " << serial_number << " with ID " << id << " constructed" << endl;
    }
    
    ~Thing()
    {
        cout << "Thing " << serial_number << " with ID " << id << " destroyed" << endl;
    }

	// copy constructor blows up on countdown and throws an exception
    Thing(const Thing& original) : id(original.id)
    {
        count_down--;
        if(count_down == 0) {
            cout << "Attempt to copy construct Thing " << object_counter + 1 << " with ID " << id << " failed!!!" << endl;
            throw Thing_exception("Boom!");
            }
        // only now increment the object counter and save the serial number
        serial_number = ++object_counter;   // first one is one
        cout << "Thing " << serial_number << " with ID " << id << " copy constructed" << endl;
    }
    
	// no need for move construction
    Thing(Thing&& original) = delete;
    
    // use copy-swap to get the strong exception guarantee
    Thing& operator= (const Thing& rhs)
    {
        Thing tmp(rhs);
        swap(tmp);
        cout << "Thing " << serial_number << " with ID " << id << " copy assigned to" << endl;
        return *this;
    }
	
	// no need for move assignment
    Thing& operator= (Thing&& rhs) = delete;

    // set the count-down counter
    static void set_count_down(int count_down_)
        {count_down = count_down_;}
	int get_ID() const
		{return id;}
	bool operator< (const Thing& rhs) const
		{return id < rhs.id;}
    void swap(Thing& other)
        {
            // only built-in types involved, so std::swap does not move anything.
            std::swap(id, other.id);
            std::swap(serial_number, other.serial_number);
        }
	friend ostream& operator<< (ostream& os, const Thing& t);
private:
	int id;
    int serial_number;
    static int object_counter;
    static int count_down;
};

int Thing::object_counter = 0;
int Thing::count_down = 100;

/* function prototypes */
ostream& operator<< (ostream& os, const Thing& t);

void demo_exception_safety();

template<typename T, typename OF>
void print(const char* label, const Ordered_list<T, OF>& in_list);


// Here we go!
int main(void)
{
	demo_exception_safety();
    
	cout << "Done!" << endl;
	return 0;
}

	
void demo_exception_safety()
{
	// use default less-than function
	typedef Ordered_list<Thing> Thing_list_t;
	/* a set of Thing objects - each Thing constructed with an ID number */
	Thing t1(101);
	Thing t2(102);
	Thing t3(103);
	Thing t4(104);
	Thing t5(105);

{   // start a local scope
	cout << "\nDemonstrate exception safety on insert" << endl;
    Thing::set_count_down(3); // blow up soon!
    
	Thing_list_t thing_list;

    try {
        // insert simply in order for clarity
        thing_list.insert(t1);
        thing_list.insert(t2);
        print("thing_list after first insertions", thing_list);
        cout << "Attempt thing_list.insert(t3);" << endl;
        thing_list.insert(t3); // should blow up when this is copied in
        print("thing_list", thing_list);
	}
    catch(Thing_exception& x)
    {
        cout << "Thing_exception caught:" << x.msg << endl;
        print("thing_list after exception caught:", thing_list);
		cout << "not damaged by failed insertion and no debris left over" << endl;
    }
    cout << "leaving local scope" << endl;
}

{
    // start a local scope
	cout << "\nDemonstrate exception safety on list copy construction" << endl;
    Thing::set_count_down(6); // blow up after awhile
    Thing_list_t thing_list;
    // insert simply in order for clarity
    thing_list.insert(t1);
    thing_list.insert(t2);
    thing_list.insert(t3);
    print("thing_list after insertions", thing_list);
    try {
        // construct a copy of the thing list - blows up, copy does not exist
        cout << "Attempt copy construction: Thing_list_t thing_list2(thing_list);" << endl;
        Thing_list_t thing_list2(thing_list);  // copy of third thing should fail
        print("thing_list2", thing_list2);
        }
    catch(Thing_exception& x)
    {
        cout << "Thing_exception caught:" << x.msg << endl;
		cout << "thing_list2 failed construction and does not exist - all debris cleaned up" << endl;
    }
    cout << "leaving local scope" << endl;
}

{
    // start a local scope
	cout << "\nDemonstrate exception safety on list copy assignment" << endl;
    Thing::set_count_down(12); // blow up after a lot
    Thing_list_t thing_list1;
    // insert simply in order for clarity
    thing_list1.insert(t1);
    thing_list1.insert(t2);
    thing_list1.insert(t3);
    print("thing_list1 after insertions", thing_list1);

    Thing_list_t thing_list2;
    // insert simply in order for clarity
    thing_list2.insert(t4);
    thing_list2.insert(t5);
    print("thing_list2 after insertions", thing_list2);
    
    try {
        Thing::set_count_down(2);
        cout << "Attempt thing_list1 = thing_list2;" << endl;
        // assign one list to another; if blows up, is lhs and rhs intact?
        thing_list1 = thing_list2; // attempt to copy second item from thing_list2 fails
        print("thing_list1", thing_list1);
        print("thing_list2", thing_list2);
    }
    catch(Thing_exception& x)
    {
        cout << "Thing_exception caught:" << x.msg << endl;
        print("thing_list1 is unchanged", thing_list1);
        print("thing_list2 is unchanged", thing_list2);
		cout << "no debris left over" << endl;
    }
    
    cout << "leaving local scope" << endl;
}

{
    // start a local scope
	cout << "\nDemonstrate list move construction" << endl;
    Thing::set_count_down(6);
    Thing_list_t thing_list;
    // insert simply in order for clarity
    thing_list.insert(t1);
    thing_list.insert(t2);
    thing_list.insert(t3);
    print("thing_list after insertions", thing_list);
    // no exceptions are possible because no data is being copied
    // using the move constructor to initialize a thing list
    cout << "Attempt move construction: Thing_list_t thing_list2(std::move(thing_list));" << endl;
    Thing_list_t thing_list2(std::move(thing_list));
    print("thing_list after move construction", thing_list);
    print("thing_list2 after move construction", thing_list2);
    cout << "leaving local scope" << endl;
}

{
    // start a local scope
	cout << "\nDemonstrate list move assignment" << endl;
    Thing::set_count_down(12);
    Thing_list_t thing_list1;
    // insert simply in order for clarity
    thing_list1.insert(t1);
    thing_list1.insert(t2);
    thing_list1.insert(t3);
    print("thing_list1 after insertions", thing_list1);

    Thing_list_t thing_list2;
    // insert simply in order for clarity
    thing_list2.insert(t4);
    thing_list2.insert(t5);
    print("thing_list2 after insertions", thing_list2);
    // no exceptions are possible because no data is being copied
    Thing::set_count_down(2);
    cout << "Attempt thing_list1 = std::move(thing_list2);" << endl;
    thing_list1 = std::move(thing_list2); // attempt to copy second item from thing_list2 fails
    print("thing_list1", thing_list1);
    print("thing_list2", thing_list2);
    
    cout << "leaving local scope" << endl;
}

    cout << "leaving main" << endl;
}



// Print the contents of the list on one line, separated by spaces.
// This requires that operator<< be defined for the type of item in the list.
template<typename T, typename OF>
void print(const char* label, const Ordered_list<T, OF>& in_list)
{
    cout << label << " has " << in_list.size() << " items:";
	for(typename Ordered_list<T, OF>::Iterator it = in_list.begin(); it != in_list.end(); it++) {
		cout << ' ' << *it;
        }
	cout << endl;
}


ostream& operator<< (ostream& os, const Thing& t)
{
	os << "Thing" << t.serial_number << "-" << t.id;
	return os;
}

/* Sample output
-bash-4.1$ g++ -std=c++11 -pedantic-errors -Wall Ordered_list_exception_safety_demo.cpp Utility.cpp p2_globals.cpp
-bash-4.1$ ./a.out
Thing 1 with ID 101 constructed
Thing 2 with ID 102 constructed
Thing 3 with ID 103 constructed
Thing 4 with ID 104 constructed
Thing 5 with ID 105 constructed

Demonstrate exception safety on insert
Thing 6 with ID 101 copy constructed
Thing 7 with ID 102 copy constructed
thing_list after first insertions has 2 items: Thing6-101 Thing7-102
Attempt thing_list.insert(t3);
Attempt to copy construct Thing 8 with ID 103 failed!!!
Thing_exception caught:Boom!
thing_list after exception caught: has 2 items: Thing6-101 Thing7-102
not damaged by failed insertion and no debris left over
leaving local scope
Thing 6 with ID 101 destroyed
Thing 7 with ID 102 destroyed

Demonstrate exception safety on list copy construction
Thing 8 with ID 101 copy constructed
Thing 9 with ID 102 copy constructed
Thing 10 with ID 103 copy constructed
thing_list after insertions has 3 items: Thing8-101 Thing9-102 Thing10-103
Attempt copy construction: Thing_list_t thing_list2(thing_list);
Thing 11 with ID 101 copy constructed
Thing 12 with ID 102 copy constructed
Attempt to copy construct Thing 13 with ID 103 failed!!!
Thing 11 with ID 101 destroyed
Thing 12 with ID 102 destroyed
Thing_exception caught:Boom!
thing_list2 failed construction and does not exist - all debris cleaned up
leaving local scope
Thing 8 with ID 101 destroyed
Thing 9 with ID 102 destroyed
Thing 10 with ID 103 destroyed

Demonstrate exception safety on list copy assignment
Thing 13 with ID 101 copy constructed
Thing 14 with ID 102 copy constructed
Thing 15 with ID 103 copy constructed
thing_list1 after insertions has 3 items: Thing13-101 Thing14-102 Thing15-103
Thing 16 with ID 104 copy constructed
Thing 17 with ID 105 copy constructed
thing_list2 after insertions has 2 items: Thing16-104 Thing17-105
Attempt thing_list1 = thing_list2;
Thing 18 with ID 104 copy constructed
Attempt to copy construct Thing 19 with ID 105 failed!!!
Thing 18 with ID 104 destroyed
Thing_exception caught:Boom!
thing_list1 is unchanged has 3 items: Thing13-101 Thing14-102 Thing15-103
thing_list2 is unchanged has 2 items: Thing16-104 Thing17-105
no debris left over
leaving local scope
Thing 16 with ID 104 destroyed
Thing 17 with ID 105 destroyed
Thing 13 with ID 101 destroyed
Thing 14 with ID 102 destroyed
Thing 15 with ID 103 destroyed

Demonstrate list move construction
Thing 19 with ID 101 copy constructed
Thing 20 with ID 102 copy constructed
Thing 21 with ID 103 copy constructed
thing_list after insertions has 3 items: Thing19-101 Thing20-102 Thing21-103
Attempt move construction: Thing_list_t thing_list2(std::move(thing_list));
thing_list after move construction has 0 items:
thing_list2 after move construction has 3 items: Thing19-101 Thing20-102 Thing21-103
leaving local scope
Thing 19 with ID 101 destroyed
Thing 20 with ID 102 destroyed
Thing 21 with ID 103 destroyed

Demonstrate list move assignment
Thing 22 with ID 101 copy constructed
Thing 23 with ID 102 copy constructed
Thing 24 with ID 103 copy constructed
thing_list1 after insertions has 3 items: Thing22-101 Thing23-102 Thing24-103
Thing 25 with ID 104 copy constructed
Thing 26 with ID 105 copy constructed
thing_list2 after insertions has 2 items: Thing25-104 Thing26-105
Attempt thing_list1 = std::move(thing_list2);
thing_list1 has 2 items: Thing25-104 Thing26-105
thing_list2 has 3 items: Thing22-101 Thing23-102 Thing24-103
leaving local scope
Thing 22 with ID 101 destroyed
Thing 23 with ID 102 destroyed
Thing 24 with ID 103 destroyed
Thing 25 with ID 104 destroyed
Thing 26 with ID 105 destroyed
leaving main
Thing 5 with ID 105 destroyed
Thing 4 with ID 104 destroyed
Thing 3 with ID 103 destroyed
Thing 2 with ID 102 destroyed
Thing 1 with ID 101 destroyed
Done!
-bash-4.1$ 
 */

