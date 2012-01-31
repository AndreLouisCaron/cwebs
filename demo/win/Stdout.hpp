#ifndef _win_Stdout_hpp__
#define _win_Stdout_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stdout.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win {

    class Transfer;

    class Stdout
    {
        /* nested types. */
    public:
        typedef ::HANDLE Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Stdout ();

    private:
        Stdout ( const Stdout& );

    public:
        ~Stdout ();

        /* methods. */
    public:
        const Handle handle () const;

        ::DWORD put ( const void * data, ::DWORD size );
        void putall ( const void * data, ::DWORD size );
        void putall ( const char * data, ::DWORD size );

        void flush ();

        /* operators. */
    private:
        Stdout& operator= ( const Stdout& );
    };

}

#endif /* _win_Stdout_hpp__ */
