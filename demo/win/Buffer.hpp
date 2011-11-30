#ifndef _win_Buffer_hpp__
#define _win_Buffer_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Buffer.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win { namespace net {

    class Buffer
    {
        /* nested types. */
    public:
        typedef ::WSABUF Data;

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Buffer ( void * data, ::u_long size );

        /* methods. */
    public:
        Data& data ();
    };

} }

#endif /* _win_Buffer_hpp__ */
