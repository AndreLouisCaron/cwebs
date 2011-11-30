#ifndef _win_Stream_hpp__
#define _win_Stream_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stream.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>
#include <string>

namespace win { namespace net {

    class Endpoint;
    class Listener;

    class Stream
    {
        /* nested types. */
    public:
        typedef ::SOCKET Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Stream ( Listener& listener );
        Stream ( Listener& listener, Endpoint& peer );

        Stream ( const Endpoint& peer );

    private:
        Stream ( const Stream& );

    public:
        ~Stream ();

        /* methods. */
    public:
        const Handle handle () const;

        int get ( void * data, int size );
        void getall ( void * data, int size );
        void getall ( char * data, int size );

        int put ( const void * data, int size );
        void putall ( const void * data, int size );
        void putall ( const char * data, int size );
        void putall ( const std::string& message );

        void shutdown ();
        void shutdowni ();
        void shutdowno ();

        /* operators. */
    private:
        Stream& operator= ( const Stream& );
    };

} }

#endif /* _win_Stream_hpp__ */
