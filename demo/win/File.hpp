#ifndef _win_InputFile_hpp__
#define _win_InputFile_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/File.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>
#include <string>

namespace win {

    class File
    {
        /* nested types. */
    public:
        typedef ::HANDLE Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        explicit File ( const std::string& path );

    private:
        File ( const File& );

    public:
        ~File ();

        /* methods. */
    public:
        const Handle handle () const;

        ::DWORD get ( void * data, ::DWORD size );
        void getall ( void * data, ::DWORD size );
        void getall ( char * data, ::DWORD size );

        /* operators. */
    private:
        File& operator= ( const File& );
    };

}

#endif /* _win_InputFile_hpp__ */
