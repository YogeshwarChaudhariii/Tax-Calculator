#ifndef PCH_H
#define PCH_H

// Workaround for Qt 6.10 bug: qrunnable.h declares a constexpr defaulted
// constructor on a class with a non-constexpr QString member, which MSVC
// correctly rejects under C++17 strict mode.
// Defining this macro before Qt headers are included suppresses the issue.
#ifdef _MSC_VER
#  pragma warning(disable: 4068)  // unknown pragma
#  define QT_NO_JAVA_STYLE_ITERATORS
// Force MSVC to treat the error as a warning, then suppress it
#  pragma warning(disable: 5297)
#endif

#include <QtWidgets>

#endif // PCH_H