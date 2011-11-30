// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Error.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Error.hpp"

namespace {

    const ::DWORD MaxMessageLength = 128;

}

namespace win {

    Error::Error ( Code code ) throw ()
        : myCode(code)
    {
    }

    Error::Code Error::code () const throw ()
    {
        return (myCode);
    }

    std::string Error::what () const
    {
        const ::LANGID language = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
        const ::DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
        ::CHAR data[::MaxMessageLength] = { 0 };
        ::DWORD size = sizeof(data)-1;
        const ::DWORD result = ::FormatMessageA
            (flags, 0, myCode, language, data, size, 0);
        // Remove trailing '\n'.
        if ( result < 2 ) {
            size = 0;
        }
        else {
            size = result-2;
        }
        data[size] = '\0';
        return (std::string(data, size));
    }

}
