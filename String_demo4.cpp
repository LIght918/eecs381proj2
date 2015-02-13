// Demonstrate compiler-supplied copy and move at work
// using a Thing class that contains a String member variable.
// The String monitoring functions shows what the compiler-supplied
// functions do with the String class member variable.

#include "String.h"
#include <iostream>

using namespace std;

// this function outputs the number and memory usage of all strings
void print_String_info();

// The class contains an int and a String ID member and
// anint ID number member initialized in the constructor.
// All of the Rule of 5 member functions are supplied by the compiler.

class Thing {
public:
	Thing(const char* id_) : id(id_), id_number(++id_counter)
    {
        cout << "Thing " << id_number << " with ID " << id_number << " constructed" << endl;
    }
    // Compiler supplies copy & move constructors and assignment operators and destructor
	friend ostream& operator<< (ostream& os, const Thing& t);
private:
	String id;
    int id_number;
    static int id_counter;
};

ostream& operator<< (ostream& os, const Thing& t)
{
	os << "Thing" << t.id_number << "-\"" << t.id << "\"";
	return os;
}

int Thing::id_counter = 0;

Thing test_fn1();
Thing test_fn2(Thing t);


int main ()
{
	String::set_messages_wanted(true);
	
    {
    Thing t1{"Xavier"};
    cout << "t1 is: " << t1 << endl;
    cout << "\nConstruct t2 from t1" << endl;
	Thing t2(t1);
    cout << "t2 is: " << t2 << endl;
    cout << "\nConstruct t3 from an unnamed temporary" << endl;
    Thing t3(Thing{"Cugat"});
    cout << "t3 is: " << t3 << endl;
    cout << "\nConstruct t4 from a function return value" << endl;
    Thing t4(test_fn1());
    cout << "t4 is: " << t4 << endl;
    cout << "\nAssign t2 = t4" << endl;
    t2 = t4;
    cout << "t2 now is: " << t2 << endl;
    
    cout << "\nAssign t3 to returned function value" << endl;
    t3 = test_fn1();
    cout << "t3 now is: " << t3 << endl;
    
    cout << "\nGive t3 to a function that returns its by value parameter and set t4 to the returned value" << endl;
    t4 = test_fn2(t3);
    cout << "t4 now is: " << t4 << endl;

    cout << "\n\nleaving scope" << endl;
    }
	

}


Thing test_fn1()
{
	cout << "in test_fn1:" << endl;
    Thing t{"Carmen"};
    return t;
}

// returning the by-value parameter variable inhibits constructor elision
// - the Returned Value Optimization is not performed.
Thing test_fn2(Thing t)
{
    cout << "In test_f2: by-value parameter t is: " << t << endl;
    cout << "Assign by-value parameter t to a different value" << endl;
    t = Thing{"Miranda"};
    cout << "t now is: " << t << endl;
    cout << "Return the by-value parameter by value" << endl;
    return t;
}
   
/* 
// Output: Note: compiler is allowed to do ctor elision
Ctor: "Xavier"
Thing 1 with ID 1 constructed
t1 is: Thing1-"Xavier"

Construct t2 from t1
Copy ctor: "Xavier"
t2 is: Thing1-"Xavier"

Construct t3 from an unnamed temporary
Ctor: "Cugat"
Thing 2 with ID 2 constructed
t3 is: Thing2-"Cugat"

Construct t4 from a function return value
in test_fn1:
Ctor: "Carmen"
Thing 3 with ID 3 constructed
t4 is: Thing3-"Carmen"

Assign t2 = t4
Copy assign from String:  "Carmen"
Copy ctor: "Carmen"
Dtor: "Xavier"
t2 now is: Thing3-"Carmen"

Assign t3 to returned function value
in test_fn1:
Ctor: "Carmen"
Thing 4 with ID 4 constructed
Move assign from String:  "Carmen"
Dtor: "Cugat"
t3 now is: Thing4-"Carmen"

Give t3 to a function that returns its by value parameter and set t4 to the returned value
Copy ctor: "Carmen"
In test_f2: by-value parameter t is: Thing4-"Carmen"
Assign by-value parameter t to a different value
Ctor: "Miranda"
Thing 5 with ID 5 constructed
Move assign from String:  "Miranda"
Dtor: "Carmen"
t now is: Thing5-"Miranda"
Return the by-value parameter by value
Move ctor: "Miranda"
Move assign from String:  "Miranda"
Dtor: "Carmen"
Dtor: ""
t4 now is: Thing5-"Miranda"


leaving scope
Dtor: "Miranda"
Dtor: "Carmen"
Dtor: "Carmen"
Dtor: "Xavier"
*/


