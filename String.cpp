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
void String::deconstruct()
{
    total_allocation -= allocation;
    if (data != nullptr && data != &a_null_byte)
    {
        delete[] data;
    }
    data = &a_null_byte;
}

// allocates a new char * and keeps track of total_allocation
char* String::allocate(int n)
{
    total_allocation += n;
    char* new_data = new char[n];
    memset(new_data, '\0', n);
    return new_data;
}

// resizes the string to handle if the length was increased by n characters
void String::resize(int n)
{
    assert(n > 0);
    if (allocation == 0)
    {
        allocation = n + 1;
        data = allocate(allocation);
        length = 0;
        return;
    }
    assert(allocation > 0);
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
String& String::copy(const String& rhs)
{
    String temp;
    if (rhs.allocation > 0)
    {
        temp.data = allocate(rhs.allocation);
        memcpy(temp.data, rhs.data, rhs.allocation);
        temp.allocation = rhs.allocation;
        temp.length = rhs.length;
    }
    swap(temp);
    return *this;
}

// Default initialization is to contain an empty string with no allocation.
// If a non-empty C-string is supplied, this String gets minimum allocation.
String::String(const char* cstr_)
{
    number++;
    if (messages_wanted)
    {
        std::cout << "Ctor: \"" << cstr_ << "\"" << "\n";
    }
    data = &a_null_byte;
    allocation = 0;
    length = 0;
    int cstrlength = strlen(rhs);
    if (cstrlength > 0)
    {
        data = allocate(cstrlength + 1);
        strcpy(temp.data, rhs);
        length = cstrlength;
        allocation = length + 1;
    }
}
// The copy constructor initializes this String with the original's data,
// and gets minimum allocation.
String::String(const String& original)
{
    number++;
    if (messages_wanted)
    {
        std::cout << "Copy ctor: \"" << original << "\"" << "\n";
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
        std::cout << "Move ctor: \"" << original << "\"" << "\n";
    }
    swap(original);
}
// deallocate C-string memory
String::~String() noexcept
{
    number--;
    if (messages_wanted)
    {
        std::cout << "Dtor: \"" << *this << "\"" << "\n";
    }
    deconstruct();
}

// Assignment operators
// Left-hand side gets a copy of rhs data and gets minimum allocation.
// This operator use the copy-swap idiom for assignment.
String& String::operator= (const String& rhs)
{
    if (messages_wanted)
    {
        std::cout << "Copy assign from String:  \"" << rhs << "\"" << "\n";
    }
    copy(rhs);
    return *this;
}
// This operator creates a temporary String object from the rhs C-string, and swaps the contents
String& String::operator= (const char* rhs)
{
    if (messages_wanted)
    {
        std::cout << "Assign from C-string:  \"" << rhs << "\"" << "\n";
    }
    String temp(rhs);
    swap(temp);
    return *this;
}
// Move assignment - simply swaps contents with rhs without any copying
String& String::operator= (String&& rhs) noexcept
{
    if (messages_wanted)
    {
        std::cout << "Move assign from String:  \"" << rhs << "\"" << "\n";
    }
    swap(rhs);
    return *this;
}

// Accesssors

// checks the subscript i and throws errors if i is out of bounds
void String::check_subscript(int i) const
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
    return data[i];
}
const char& String::operator[] (int i) const    // const version for const Strings
{
    check_subscript(i);
    return data[i];
}


/* Return a String starting with i and extending for len characters
The substring must be contained within the string.
Values of i and len for valid input are as follows:
i >= 0 && len >= 0 && i <= size && (i + len) <= size.
If both i = size and len = 0, the input is valid and the result is an empty string.
Throw exception if the input is invalid. */
String String::substring(int i, int len) const
{
    if (!(i >= 0 && len >= 0 && i <= length && (i + len) <= length))
    {
        throw String_exception("Substring bounds invalid");
    }
    String result(*this);
    result.remove(0, i);
    result.remove(len, length - len);
    return result;
}

// Modifiers
// Set to an empty string with minimum allocation by create/swap with an empty string.
void String::clear()
{
    *this = String();
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
    strcpy(data + i, data + i + len);
    length -= len;
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
    memmove(data + i + src.length, data + i, length - i);
    strncpy(data + i, src.data, src.length);
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
    return *this;
}
String& String::operator += (const char* rhs)
{
    int added_size = strlen(rhs);
    resize(added_size);
    strcpy(data + length, rhs);
    length += added_size;
    return *this;
}
String& String::operator += (const String& rhs)
{
    resize(rhs.length);
    strcpy(data + length, rhs.data);
    length += rhs.length;
    return *this;
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

// non-member overloaded operators

// compare lhs and rhs strings; constructor will convert a C-string literal to a String.
// comparison is based on std::strcmp result compared to 0
bool operator== (const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}
bool operator!= (const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) != 0;
}
bool operator< (const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}
bool operator> (const String& lhs, const String& rhs)
{
    return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}

/* Concatenate a String with another String.
 If one of the arguments is a C-string, the String constructor will automatically create
 a temporary String for it to match this function (inefficient, but instructive).
 This automatic behavior would be disabled if the String constructor was declared "explicit".
 This function constructs a copy of the lhs in a local String variable,
 then concatenates the rhs to it with operator +=, and returns it. */
String operator+ (const String& lhs, const String& rhs)
{
    String sum(lhs);
    sum += rhs;
    return sum;
}

// Input and output operators and functions
// The output operator writes the contents of the String to the stream
std::ostream& operator<< (std::ostream& os, const String& str)
{
    os << str.c_str();
    return os;
}

/* The input operator clears the supplied String, then starts reading the stream.
It skips initial whitespace, then copies characters into
the supplied str until whitespace is encountered again. The terminating
whitespace remains in the input stream, analogous to how string input normally works.
str is expanded as needed, and retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
std::istream& operator>> (std::istream& is, String& str)
{
    str.clear();
    bool leading = true;
    bool trailing = false;
    while (leading || !trailing)
    {
        char next;
        if (!(is.get(next)))
        {
            if (is.eof()) break;
            throw String_exception(">> error");
        }
        std::cerr << next;
        if (!isspace(next))
        {
            str += next;
            leading = false;
        }
        else
        {
            if (!leading) trailing = true;
        }
    }
    std::cerr << "escaped >>" << std::endl;
    std::cerr << "data is " << str << std::endl;
    is.unget();
    return is;
}

/* getline for String clears str to an empty String, then reads characters into str until it finds a '\n',
which is left in the stream (this differs from the fgets and std::getline functions).
str's allocation is expanded as needed, and it retains the final allocation.
If the input stream fails, str contains whatever characters were read. */
std::istream& getline(std::istream& is, String& str)
{
    str.clear();
    while (true)
    {
        char next;
        if (!(is.get(next)))
        {
            if (is.eof()) break;
            throw String_exception("getline failure");
        }
        std::cerr << next;
        if (next != '\n')
        {
            str += next;
        }
        else
        {
            break;
        }
    }
    std::cerr << "escaped getline" << std::endl;
    std::cerr << "data is " << str << std::endl;
    is.unget();
    return is;
}