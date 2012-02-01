#ifndef _win_Stream_hpp__
#define _win_Stream_hpp__

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
 * @file demo/win/Stream.hpp
 */

#include <WinSock2.h>
#include <Windows.h>
#include <string>

namespace win { namespace net {

    class Buffer;
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
