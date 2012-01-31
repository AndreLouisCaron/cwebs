// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stdout.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Stdout.hpp"
#include "Error.hpp"

namespace win {

    Stdout::Stdout ()
        : myHandle(::GetStdHandle(STD_OUTPUT_HANDLE))
    {
    }

    Stdout::~Stdout ()
    {
    }

    const Stdout::Handle Stdout::handle () const
    {
        return (myHandle);
    }

    ::DWORD Stdout::put ( const void * data, ::DWORD size )
    {
        ::DWORD read = 0;
        const ::BOOL result = ::WriteFile(
            handle(), data, size, &read, 0
            );
        if ( result == 0 ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(WriteFile, error);
        }
        return (read);
    }

    void Stdout::putall ( const void * data, ::DWORD size )
    {
        putall(static_cast<const char*>(data), size);
    }

    void Stdout::putall ( const char * data, ::DWORD size )
    {
        ::DWORD used = 0;
        ::DWORD pass = 0;
        do {
            pass = put(data+used, size-used);
        }
        while ((pass > 0) && ((used+=pass) < size));
    }

    void Stdout::flush ()
    {
        const ::BOOL result = ::FlushFileBuffers(handle());
        if ( result == FALSE ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(FlushFileBuffers, error);
        }
    }

}
