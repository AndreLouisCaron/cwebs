#ifndef _win_Endpoint_hpp__
#define _win_Endpoint_hpp__

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
 * @file demo/win/Endpoint.hpp
 */

#include <WinSock2.h>
#include <Windows.h>
#include <iosfwd>

namespace win { namespace net {

    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;

    class Endpoint
    {
        /* nested types. */
    public:
        typedef ::sockaddr_in Data;

        /* class methods. */
    public:
        static const Endpoint resolve ( const char * name, uint16_t port );

        static const Endpoint any ( uint16_t port );
        static const Endpoint localhost ( uint16_t port );

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Endpoint ();
        Endpoint ( const Data& data );
        Endpoint ( const Data& data, uint16_t port );
        Endpoint ( uint32_t address, uint16_t port );
        Endpoint
            ( uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port );

        Endpoint ( const Endpoint& other );

        /* methods. */
    public:
        int size () const;

        ::sockaddr * raw ();
        const ::sockaddr * raw () const;

        Data& data ();
        const Data& data () const;

        uint16_t port () const;
    };

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint );

} }

#endif /* _win_Endpoint_hpp__ */
