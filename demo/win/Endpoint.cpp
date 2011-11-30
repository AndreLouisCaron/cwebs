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

namespace win { namespace net {

    Endpoint::Endpoint ()
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
    }

    Endpoint::Endpoint ( const Data& address )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
    }

    Endpoint::Endpoint ( uint32_t address, uint16_t port )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.sin_family = AF_INET;
        myData.sin_addr.S_un.S_addr = ::htonl(address);
        myData.sin_port = ::htons(port);
    }

    Endpoint::Endpoint ( uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port )
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
