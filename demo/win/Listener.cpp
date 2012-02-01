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
 * @file demo/win/Listener.cpp
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
