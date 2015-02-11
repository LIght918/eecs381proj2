#ifndef UTILITY_H
#define UTILITY_H

#include "String.h"

/* Utility functions, constants, and classes used by more than one other modules */

// a simple class for error exceptions - msg points to a C-string error message
struct Error {
	Error(const char* msg_ = "") :
		msg(msg_)
		{}
	const char* msg;
};

void throw_file_error();

int integer_read();

String& title_read();

String& parse_title(String& title_string);

#endif