#ifndef _nix_Endpoint_hpp__
#define _nix_Endpoint_hpp__

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
// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)

/*!
 * @file nix/Endpoint.hpp
 * @brief IPv4 endpoint.
 */

#include <stdint.h>
#include <netinet/in.h>
#include <iosfwd>

namespace nix { namespace net {

    class Endpoint
    {
        /* data. */
    private:
        ::sockaddr_in myData;

        /* class methods. */
    public:
        static const Endpoint resolve ( const char * name );
        static const Endpoint resolve ( const char * name, uint16_t port );
        static const Endpoint resolve ( const char * name, const char * port );

        static const Endpoint any ( uint16_t port );
        static const Endpoint localhost ( uint16_t port );

        /* construction. */
    public:
        Endpoint ( const ::sockaddr_in& data );
        Endpoint ( const ::sockaddr_in& data, uint16_t port );

        Endpoint ( uint16_t port )
        {
            myData.sin_family = AF_INET;
            myData.sin_addr.s_addr = INADDR_ANY;
            myData.sin_port = ::htons(port);
        }

        Endpoint ( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port )
        {
            myData.sin_family = AF_INET;
            myData.sin_addr.s_addr =
                (uint32_t(d) << 24)|
                (uint32_t(c) << 16)|
                (uint32_t(b) <<  8)|
                (uint32_t(a) <<  0);
            myData.sin_port = ::htons(port);
        }

        /* methods. */
    public:
        ::sockaddr_in& data ()
        {
            return (myData);
        }

        const ::sockaddr_in& data () const
        {
            return (myData);
        }
    };

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint );

} }

#endif /* _nix_Endpoint_hpp__ */
