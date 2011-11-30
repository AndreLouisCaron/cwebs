// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Listener.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Listener.hpp"
#include "Error.hpp"
#include "Endpoint.hpp"

namespace {

    ::SOCKET allocate ()
    {
#if 0
        const ::SOCKET result = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
        const ::SOCKET result = ::WSASocket
            (AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
#endif
        if ( result == INVALID_SOCKET )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(socket, error);
        }
        return (result);
    }

}

namespace win { namespace net {

    Listener::Listener ( const Endpoint& name, int backlog )
        : myHandle(::allocate())
    {
        const ::sockaddr_in& address = name.data();
        const int result = ::bind(
            handle(), (::sockaddr*)&address, sizeof(address)
            );
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(bind, error);
        }
        if ( ::listen(handle(), backlog) == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(listen, error);
        }
    }

    Listener::~Listener ()
    {
        const int result = ::closesocket(myHandle);
        if ( result == SOCKET_ERROR )
        {
            const win::Error error(::WSAGetLastError());
            //std::cerr << "[[[ " << error << " ]]]" << std::endl;
        }
    }

    const Listener::Handle Listener::handle () const
    {
        return (myHandle);
    }

} }
