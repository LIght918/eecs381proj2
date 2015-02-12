#include <iostream>
#include <fstream>
#include <string.h>
#include "String.h"
#include <assert.h>
#include <cctype>
#include "Utility.h"

char String::a_null_byte = '\0';	// to hold a null byte for empty string representation

/* Variables for monitoring functions - not part of a normal implementation. */
/* But used here for demonstration and testing purposes. */
int String::number = 0;				// counts number of String objects in existence
int String::total_allocation = 0;	// counts total amount of memory allocated
bool String::messages_wanted = false;	// whether to output constructor/destructor/operator= messages, initially false

// deallocates data
void deconstruct()
{
    total_allocation -= allocation;
    if (data != nullptr && data != a_null_byte)
    {
        delete[] data;
    }
    data = a_null_byte;
}

// allocates a new char * and keeps track of total_allocation
char* allocate(int n)
{
    total_allocation += n + 1;
    return new char[n + 1];
}

// resizes the string to handle if the length was increased by n characters
void resize(int n)
{
    assert(n > 0);
    if (allocation == 0)
    {
        data = allocate(n + 1);
        length = 0;
        allocation = n + 1;
        return;
    }
    assert(allocation > 0 && length > 0);
    if (allocation < length + n + 1)
    {
        allocation = 2 * (length + n + 1);
        char* new_data = allocate(allocation);
        strcpy(new_data, data);
        deconstruct();
        data = new_data;
    }
}

// copies the rhs into this string
String& copy(String& rhs)
{
    deconstruct();
    if (original.length > 0)
    {
        allocation = original.allocation;
        data = allocate(allocation);
        strcpy(data, original.data);
        length = original.length;
    }
    else
    {
        length = 0;
        allocation = 0;
    }
    return *this;
}

// copies the rhs into this string
String& copy(const char* rhs)
{
    deconstruct();
    int cstrlength = strlen(cstr_);
    if (cstrlength > 0)
    {
        data = allocate(cstrlength + 1);
        strcpy(data, cstr_);
        length = cstrlength;
        allocation = length + 1;
    }
    else
    {
        length = 0;
        allocation = 0;
    }
    return *this;
}

// moves rhs into this string
String& move(const char* rhs)
{
    deconstruct();
    data = rhs;
    length = strlen(rhs);
    allocation = length + 1;
    return *this;
}

// moves rhs into this string
String& move(String&& rhs)
{
    swap(rhs);
    return *this;
}

// Default initialization is to contain an empty string with no allocation.
// If a non-empty C-string is supplied, this String gets minimum allocation.
String::String(const char* cstr_ = "")
{
    number++;
    if (messages_wanted)
    {
        cout << "Ctor: \"" + cstr_ + "\"";
    }
    copy(rhs);
}
// The copy constructor initializes this String with the original's data,
// and gets minimum allocation.
String::String(const String& original)
{
    number++;
    if (messages_wanted)
    {
        cout << "Copy ctor: \"" + original + "\"";
    }
    copy(original);
}
// Move constructor - take original's data, and set the original String
// member variables to the empty state (do not initialize "this" String and swap).
String::String(String&& original) noexcept
{
    number++;
    if (messages_wanted)
    {
        cout << "Move ctor: \"" + original + "\"";
    }
    move(original);
}
// deallocate C-string memory
~String::String() noexcept
{
    number--;
    deconstruct();
}

// Assignment operators
// Left-hand side gets a copy of rhs data and gets minimum allocation.
// This operator use the copy-swap idiom for assignment.
String& String::operator= (const String& rhs)
{
    if (messages_wanted)
    {
        cout << "Copy assign from String:  \"" + rhs + "\"";
    }
    copy(rhs);
    return *this;
}
// This operator creates a temporary String object from the rhs C-string, and swaps the contents
String& String::operator= (const char* rhs)
{
    if (messages_wanted)
    {
        cout << "Assign from C-string:  \"" + rhs + "\"";
    }
    move(rhs);
    return *this;
}
// Move assignment - simply swaps contents with rhs without any copying
String& String::operator= (String&& rhs) noexcept
{
    if (messages_wanted)
    {
        cout << "Move assign from String:  \"" + rhs + "\"";
    }
    move(rhs);
    return *this;
}

// Accesssors
// Return a pointer to the internal C-string
const char* String::c_str() const
{return data;}
// Return size (length) of internal C-string in this String
int String::size() const
{return length;}
// Return current allocation for this String
int String::get_allocation() const
{return allocation;}

// checks the subscript i and throws errors if i is out of bounds
void check_subscript(int i)
{
    if (i < 0 || i >= length)
    {
        throw String_exception("Subscript out of range");
    }
}
// Return a reference to character i in the string.
// Throw exception if 0 <= i < size is false.
char& String::operator[] (int i)
{
    check_subscript(i);
    return &data[i];
}
const char& String::operator[] (int i) const    // const version for const Strings
{
    check_subscript(i);
    return &data[i];
}


/* Return a String starting with i and extending for len characters
The substring must be contained within the string.
Values of i and len for valid input are as follows:
i >= 0 && len >= 0 && i <= size && (i + len) <= size.
If both i = size and len = 0, the input is valid and the result is an empty string.
Throw exception if the input is invalid. */
String String::substring(int i, int len) const
{
    if (!(i >= 0 && len >= 0 && i <= size && (i + len) <= size))
    {
        throw String_exception("Substring bounds invalid");
    }
    char sub[len + 1];
    strcpy(sub, data + i);
    return String(sub);
}

// Modifiers
// Set to an empty string with minimum allocation by create/swap with an empty string.
void String::clear()
{
    move("");
}

/* Remove the len characters starting at i; allocation is unchanged.
The removed characters must be contained within the String.
Valid values for i and len are the same as for substring. */
void String::remove(int i, int len)
{
    if (i >= length || i + len >= length)
    {
        throw String_exception("Remove bounds invalid");
    }
    char* new_data = new char[length - len];
    strncpy(new_data, data, i);
    strcpy(new_data + i, data + i + len);
}

/* Insert the supplied source String before character i of this String,
pushing the rest of the contents back, reallocating as needed.
If i == size, the inserted string is added to the end of this String.
This function does not create any temporary String objects.
It either directly inserts the new data into this String's space if it is big enough,
or allocates new space and copies in the old data with the new data inserted.
This String retains the final allocation.
Throw exception if 0 <= i <= size is false. */
void String::insert_before(int i, const String& src)
{
    if (!(i >= 0 && i <= length))
    {
        throw String_exception("Insertion point out of range");
    }
    if (i == length)
    {
        *this += src;
        return;
    }
    resize(src.length);
    memmove(data + i + src.length, length - i);
    strcpy(data + i, src.data, src.length);
    length += src.length;
}

/* These concatenation operators add the rhs string data to the lhs object.
They do not create any temporary String objects. They either directly copy the rhs data
into the lhs space if it is big enough to hold the rhs, or allocate new space
and copy the old lhs data into it followed by the rhs data. The lhs object retains the
final memory allocation. If the rhs is a null byte or an empty C-string or String,
no change is made to lhs String. */
String& String::operator += (char rhs)
{
    resize(1);
    data[length++] = rhs;
}
String& String::operator += (const char* rhs)
{
    int added_size = strlen(rhs);
    resize(added_size);
    strcpy(data + length, rhs);
    length += added_size;
}
String& String::operator += (const String& rhs)
{
    resize(rhs.length);
    strcpy(data + length, rhs.data);
    length += rhs.length;
}

/* Swap the contents of this String with another one.
The member variable values are interchanged, along with the
pointers to the allocated C-strings, but the two C-strings
are neither copied nor modified. No memory allocation/deallocation is done. */
void String::swap(String& other) noexcept
{
    char* temp_data = data;
    int temp_length = length;
    int temp_alloc = allocation;
    data = other.data;
    length = other.length;
    allocation = temp_alloc;
    other.data = temp_data;
    other.length = temp_length;
    other.allocation = temp_alloc;
}

/* Monitoring functions - not part of a normal implementation */
/*	used here for demonstration and testing purposes. */

// Return the total number of Strings in existence
static int String::get_number()
{return number;}
// Return total bytes allocated for all Strings in existence
static int String::get_total_allocation()
{return total_allocation;}
// Call with true to cause ctor, assignment, and dtor messages to be output.
// These messages are output from each function before it does anything else.
static void String::set_messages_wanted(bool messages_wanted_)
{messages_wanted = messages_wanted_;}

// non-member overloaded operators

// compare lhs and rhs strings; constructor will convert a C-string literal to a String.
// comparison is based on std::strcmp result compared to 0
bool String::operator== (const String& lhs, const String& rhs)
{
    return strcmp(lhs.data, rhs.data) == 0;
}
bool String::operator!= (const String& lhs, const String& rhs)
{
    return strcmp(lhs.data, rhs.data) != 0;
}
bool String::operator< (const String& lhs, const String& rhs)
{
    return strcmp(lhs.data, rhs.data) < 0;
}
bool String::operator> (const String& lhs, const String& rhs)
{
    return strcmp(lhs.data, rhs.data) > 0;
}

/* Concatenate a String with another String.
 If one of the arguments is a C-string, the String constructor will automatically create
 a temporary String for it to match this function (inefficient, but instructive).
 This automatic behavior would be disabled if the String constructor was declared "explicit".
 This function constructs a copy of the lhs in a local String variable,
 then concatenates the rhs to it with operator +=, and returns it. */
String String::operator+ (const String& lhs, const String& rhs)
{
    String sum(lhs);
    sum += rhs;
    return sum;
}

// Input and output operators and functions
// The output operator writes the contents of the String to the stream
std::ostream& String::operator<< (std::ostream& os, const String& str)
{
    os << str.data;
    return os;
}

/* The input operator clears the supplied String, then starts reading the stream.
It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The terminating
whitespace remains in the input stream, analogous to how string input normally works.
str is expanded as needed, and retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
std::istream& String::operator>> (std::istream& is, String& str)
{
    clear(str);
    bool leading = true;
    bool trailing = false;
    while (leading || !trailing)
    {
        char next;
        if (!(is >> next))
        {
            throw String_exception();
        }
        if (!isspace(next))
        {
            str += next;
            leading = false;
            trailing = false;
        }
        else if (!leading)
        {
            trailing = true;
        }
        else
        {
            leading = false;
        }
    }
    is.unget();
    return is;
}

/* getline for String clears str to an empty String, then reads characters into str until it finds a '\n',
which is left in the stream (this differs from the fgets and std::getline functions).
str's allocation is expanded as needed, and it retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
std::istream& String::getline(std::istream& is, String& str)
{
    clear(str);
    while (true)
    {
        char next;
        if (!(is >> next))
        {
            throw String_exception();
        }
        if (next != '\n')
        {
            str += next;
        }
        else
        {
            break;
        }
    }
    is.unget();
    return is;
}