// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/File.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
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
