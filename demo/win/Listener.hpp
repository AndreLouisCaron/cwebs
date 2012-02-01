#ifndef _win_Listener_hpp__
#define _win_Listener_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Listener.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win { namespace net {

    class Endpoint;

    class Listener
    {
        /* nested types. */
    public:
        typedef ::SOCKET Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Listener ( const Endpoint& host, int backlog=SOMAXCONN );

    private:
        Listener ( const Listener& );

    public:
        ~Listener ();

        /* methods. */
    public:
        const Handle handle () const;

        /* operators. */
    private:
        Listener& operator= ( const Listener& );
    };

} }

#endif /* _win_Listener_hpp__ */
