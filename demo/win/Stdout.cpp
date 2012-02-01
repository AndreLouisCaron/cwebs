// Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
        if ( result == FALSE )
        {
            const ::DWORD error = ::GetLastError();
            // Console device can't be flushed.  Don't
            // crash if the device is not a regular file.
            if (error != ERROR_INVALID_HANDLE) {
                UNCHECKED_WIN32C_ERROR(FlushFileBuffers, error);
            }
        }
    }

}
