// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Endpoint.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Endpoint.hpp"
#include "Error.hpp"

#include <ws2tcpip.h>

namespace win { namespace net {

    const Endpoint Endpoint::resolve ( const char * name )
    {
        ::addrinfo hint; ::ZeroMemory(&hint, sizeof(hint));
        hint.ai_family   = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = IPPROTO_TCP;
        ::addrinfo * peer = 0;
        const int result = ::getaddrinfo(name, "http", &hint, &peer);
        if (result != 0) {
            UNCHECKED_WIN32C_ERROR(getaddrinfo, result);
        }
        const win::net::Endpoint endpoint(
            *reinterpret_cast<const ::sockaddr_in*>(peer->ai_addr));
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

} }
