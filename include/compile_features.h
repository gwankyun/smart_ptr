#pragma once

#ifndef CXX_NOEXCEPT
#if defined(_MSC_VER) && _MSC_VER >= 1900 // VS2015
#define CXX_NOEXCEPT
#endif
#endif // !CXX_NOEXCEPT

#ifndef CXX_DEFAULTED_FUNCTIONS
#if defined(_MSC_VER) && _MSC_VER >= 1800 // VS2013
#define CXX_DEFAULTED_FUNCTIONS
#endif
#endif // !CXX_DEFAULTED_FUNCTIONS

#ifndef CXX_DELETED_FUNCTIONS
#if defined(_MSC_VER) && _MSC_VER >= 1800 // VS2013
#define CXX_DELETED_FUNCTIONS
#endif
#endif // !CXX_DELETED_FUNCTIONS

#ifndef CXX_STRONG_ENUMS
#if defined(_MSC_VER) && _MSC_VER >= 1700 // VS2012
#define CXX_STRONG_ENUMS
#endif
#endif // !CXX_STRONG_ENUMS

#ifndef CXX_OVERRIDE
#if defined(_MSC_VER) && _MSC_VER >= 1700 // VS2012
#define CXX_OVERRIDE
#endif
#endif // !CXX_OVERRIDE

#ifndef CXX_NULLPTR
#if defined(_MSC_VER) && _MSC_VER >= 1600 // VS2010
#define CXX_NULLPTR
#endif
#endif // !CXX_NULLPTR
