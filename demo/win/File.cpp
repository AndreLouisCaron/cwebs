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
 * @file demo/win/File.cpp
 */

#include "File.hpp"
#include "Error.hpp"

namespace {

    ::HANDLE open ( ::LPCSTR path, ::DWORD sharing, ::DWORD mode )
    {
        const ::DWORD flags = FILE_FLAG_OVERLAPPED;
        const ::HANDLE handle = ::CreateFileA(
            path, GENERIC_READ, sharing, 0, mode, flags, 0
            );
        if ( handle == INVALID_HANDLE_VALUE ) {
            UNCHECKED_WIN32C_ERROR(CreateFile,::GetLastError());
        }
        return (handle);
    }

}

namespace win {

    File::File ( const std::string& path )
        : myHandle(::open(path.c_str(), FILE_SHARE_READ, OPEN_EXISTING))
    {
    }

    File::~File ()
    {
        const ::BOOL result = ::CloseHandle(myHandle);
        if ( result == FALSE ) {
            const win::Error error(::GetLastError());
            //std::cerr << "[[[ " << error << " ]]]" << std::endl;
        }
    }

    const File::Handle File::handle () const
    {
        return (myHandle);
    }

    ::DWORD File::get ( void * data, ::DWORD size )
    {
        ::DWORD read = 0;
        const ::BOOL result = ::ReadFile(
            handle(), data, size, &read, 0
            );
        if ( result == 0 ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(ReadFile, error);
        }
        return (read);
    }

    void File::getall ( void * data, ::DWORD size )
    {
        getall(static_cast<char*>(data), size);
    }

    void File::getall ( char * data, ::DWORD size )
    {
        int used = 0;
        int pass = 0;
        do {
            pass = get(data+used, size-used);
        }
        while ((pass > 0) && ((used+=pass) < size));
    }

}
