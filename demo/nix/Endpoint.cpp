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
 * @file nix/Endpoint.cpp
 * @brief IPv4 endpoint.
 */

#include "Endpoint.hpp"
#include "Error.hpp"
#include <netdb.h>
#include <string.h>
#include <ostream>

namespace nix { namespace net {

    const Endpoint Endpoint::resolve ( const char * name )
    {
        return (resolve(name, "80"));
    }

    const Endpoint Endpoint::resolve ( const char * name, uint16_t port )
    {
        ::addrinfo hint; ::memset(&hint, 0, sizeof(hint));
        hint.ai_family   = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = IPPROTO_TCP;
        ::addrinfo * peer = 0;
        const int result = ::getaddrinfo(name, 0, &hint, &peer);
        if (result != 0) {
            throw Error(result);
        }
        const Endpoint endpoint(
            *reinterpret_cast<const ::sockaddr_in*>(peer->ai_addr), port);
        ::freeaddrinfo(peer);
        return (endpoint);
    }

    const Endpoint Endpoint::resolve ( const char * name, const char * port )
    {
        ::addrinfo hint; ::memset(&hint, 0, sizeof(hint));
        hint.ai_family   = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = IPPROTO_TCP;
        ::addrinfo * peer = 0;
        const int result = ::getaddrinfo(name, port, &hint, &peer);
        if (result != 0) {
            throw Error(result);
        }
        const Endpoint endpoint(
            *reinterpret_cast<const ::sockaddr_in*>(peer->ai_addr));
        ::freeaddrinfo(peer);
        return (endpoint);
    }

    const Endpoint Endpoint::any ( uint16_t port )
    {
        ::sockaddr_in data;
        ::memset(&data, 0, sizeof(data));
        data.sin_family = AF_INET;
        data.sin_addr.s_addr = INADDR_ANY;
        data.sin_port = htons(port);
        return (Endpoint(data));
    }

    const Endpoint Endpoint::localhost ( uint16_t port )
    {
        return (Endpoint(127,0,0,1,port));
    }

    Endpoint::Endpoint ( const ::sockaddr_in& data )
    {
        ::memcpy(&myData, &data, sizeof(myData));
    }

    Endpoint::Endpoint ( const ::sockaddr_in& data, uint16_t port )
    {
        ::memcpy(&myData, &data, sizeof(myData));
        myData.sin_port = htons(port);
    }

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint )
    {
      const ::sockaddr_in& data = endpoint.data();
      stream
          << ((data.sin_addr.s_addr >>  0) & 0xff) << '.'
          << ((data.sin_addr.s_addr >>  8) & 0xff) << '.'
          << ((data.sin_addr.s_addr >> 16) & 0xff) << '.'
          << ((data.sin_addr.s_addr >> 24) & 0xff) << ':'
          << ntohs(data.sin_port);
      return (stream);
    }

} }
