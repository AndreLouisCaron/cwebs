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
 * @file demo/win/Endpoint.cpp
 */

#include "Endpoint.hpp"
#include "Error.hpp"

#include <ws2tcpip.h>
#include <ostream>

namespace win { namespace net {

    const Endpoint Endpoint::resolve ( const char * name, uint16_t port )
    {
        ::addrinfo hint; ::ZeroMemory(&hint, sizeof(hint));
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
    const Endpoint Endpoint::any ( uint16_t port )
    {
        ::sockaddr_in data;
        ::ZeroMemory(&data, sizeof(data));
        data.sin_family = AF_INET;
        data.sin_addr.S_un.S_addr = INADDR_ANY;
        data.sin_port = ::htons(port);
        return (Endpoint(data));
    }

    const Endpoint Endpoint::localhost ( uint16_t port )
    {
        return (Endpoint(127,0,0,1,port));
    }

    Endpoint::Endpoint ()
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
    }

    Endpoint::Endpoint ( const Data& data )
    {
        ::CopyMemory(&myData, &data, sizeof(myData));
    }

    Endpoint::Endpoint ( const Data& data, uint16_t port )
    {
        ::CopyMemory(&myData, &data, sizeof(myData));
        myData.sin_port = ::htons(port);
    }

    Endpoint::Endpoint ( uint32_t address, uint16_t port )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
        myData.sin_addr.S_un.S_addr = ::htonl(address);
        myData.sin_port = ::htons(port);
    }

    Endpoint::Endpoint
        ( uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
        myData.sin_addr.S_un.S_un_b.s_b1 = b1;
        myData.sin_addr.S_un.S_un_b.s_b2 = b2;
        myData.sin_addr.S_un.S_un_b.s_b3 = b3;
        myData.sin_addr.S_un.S_un_b.s_b4 = b4;
        myData.sin_port = ::htons(port);
    }

    Endpoint::Endpoint ( const Endpoint& other )
    {
        ::CopyMemory(&myData, &other.myData, sizeof(myData));
    }

    Endpoint::Data& Endpoint::data ()
    {
        return (myData);
    }

    const Endpoint::Data& Endpoint::data () const
    {
        return (myData);
    }

    ::sockaddr * Endpoint::raw ()
    {
        return (reinterpret_cast<::sockaddr*>(&myData));
    }

    const ::sockaddr * Endpoint::raw () const
    {
        return (reinterpret_cast<const ::sockaddr*>(&myData));
    }

    int Endpoint::size () const
    {
        return (sizeof(myData));
    }

    uint16_t Endpoint::port () const
    {
        return (::ntohs(myData.sin_port));
    }

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint )
    {
        const ::sockaddr_in& data = endpoint.data();
        stream
            << int(data.sin_addr.S_un.S_un_b.s_b1) << '.'
            << int(data.sin_addr.S_un.S_un_b.s_b2) << '.'
            << int(data.sin_addr.S_un.S_un_b.s_b3) << '.'
            << int(data.sin_addr.S_un.S_un_b.s_b4) << ':'
            << ::ntohs(data.sin_port);
        return (stream);
    }

} }
