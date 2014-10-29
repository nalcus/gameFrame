#ifndef __StringHelp__
#define __StringHelp__

// from sfml book

#include <sstream>

// Since std::to_string doesn't work on MinGW we have to implement
// our own to support all platforms.
template <typename T>
std::string toString(const T& value);

#include "StringHelp.inl"


#endif // __StringHelp__
