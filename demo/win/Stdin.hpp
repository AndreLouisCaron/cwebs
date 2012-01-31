#ifndef _win_Stdin_hpp__
#define _win_Stdin_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stdin.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win {

    class Stdin
    {
        /* nested types. */
    public:
        typedef ::HANDLE Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Stdin ();

    private:
        Stdin ( const Stdin& );

    public:
        ~Stdin ();

        /* methods. */
    public:
        const Handle handle () const;

        ::DWORD get ( void * data, ::DWORD size );
        void getall ( void * data, ::DWORD size );
        void getall ( char * data, ::DWORD size );

        /* operators. */
    private:
        Stdin& operator= ( const Stdin& );
    };

}

#endif /* _win_Stdin_hpp__ */
