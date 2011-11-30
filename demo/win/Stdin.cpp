// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stdin.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Stdin.hpp"
#include "Error.hpp"

namespace win {

    Stdin::Stdin ()
        : myHandle(::GetStdHandle(STD_INPUT_HANDLE))
    {
    }

    Stdin::~Stdin ()
    {
    }

    const Stdin::Handle Stdin::handle () const
    {
        return (myHandle);
    }

    ::DWORD Stdin::get ( void * data, ::DWORD size )
    {
        ::DWORD read = 0;
        const ::BOOL result = ::ReadFile(
            handle(), data, size, &read, 0
            );
        if ( result == 0 ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(ReadStdin, error);
        }
        return (read);
    }

    void Stdin::getall ( void * data, ::DWORD size )
    {
        getall(static_cast<char*>(data), size);
    }

    void Stdin::getall ( char * data, ::DWORD size )
    {
        ::DWORD used = 0;
        ::DWORD pass = 0;
        do {
            pass = get(data+used, size-used);
        }
        while ((pass > 0) && ((used+=pass) < size));
    }

}
