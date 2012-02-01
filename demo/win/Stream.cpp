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
 * @file demo/win/Stream.cpp
 */

#include "Stream.hpp"
#include "Endpoint.hpp"
#include "Error.hpp"
#include "Listener.hpp"
#include <mswsock.h>

namespace {

        // Creates a socket for connection attempt.
    ::SOCKET allocate ()
    {
        const ::SOCKET result = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if ( result == INVALID_SOCKET ) {
            UNCHECKED_WIN32C_ERROR(socket,::WSAGetLastError());
        }
        return (result);
    }

        // Waits for a connection attempt from a client.
    ::SOCKET allocate ( ::SOCKET listener, ::sockaddr * peer, int * len )
    {
        const ::SOCKET result = ::accept(listener, peer, len);
        if ( result == INVALID_SOCKET ) {
            UNCHECKED_WIN32C_ERROR(accept,::WSAGetLastError());
        }
        return (result);
    }

    ::SOCKET allocate ( ::SOCKET listener, ::sockaddr_in& peer )
    {
        peer.sin_family = AF_INET; int len = sizeof(peer);
        const ::SOCKET result = ::accept(
            listener, reinterpret_cast<::sockaddr*>(&peer), &len);
        if ( result == INVALID_SOCKET ) {
            UNCHECKED_WIN32C_ERROR(accept,::WSAGetLastError());
        }
        return (result);
    }

    ::SOCKET allocate ( ::SOCKET listener )
    {
        const ::SOCKET result = ::accept(listener, 0, 0);
        if ( result == INVALID_SOCKET ) {
            UNCHECKED_WIN32C_ERROR(accept,::WSAGetLastError());
        }
        return (result);
    }

    ::HANDLE cast ( ::SOCKET handle )
    {
        return (reinterpret_cast< ::HANDLE >(handle));
    }

}

namespace win { namespace net {

    Stream::Stream ( Listener& listener )
        : myHandle(::allocate(listener.handle()))
    {
    }

    Stream::Stream ( Listener& listener, Endpoint& peer )
        : myHandle(::allocate(listener.handle(), peer.data()))
    {
    }

    Stream::Stream ( const Endpoint& peer )
        : myHandle(::allocate())
    {
        const ::sockaddr_in& address = peer.data();
        const int result = ::connect(
            handle(), (::sockaddr*)&address, sizeof(address)
            );
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            if ( error == WSAECONNREFUSED )
            {
            }
            UNCHECKED_WIN32C_ERROR(connect, error);
        }
    }

    Stream::~Stream ()
    {
        const int result = ::closesocket(myHandle);
        if ( result == SOCKET_ERROR )
        {
            const win::Error error(::WSAGetLastError());
            //std::cerr << "[[[ " << error << " ]]]" << std::endl;
        }
    }

    const Stream::Handle Stream::handle () const
    {
        return (myHandle);
    }

    int Stream::get ( void * buffer, int length )
    {
        const int result = ::recv
            (handle(), static_cast<char*>(buffer), length, 0);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            if ( error == WSAECONNRESET ) {
                return (0);
            }
            if ( error == WSAEWOULDBLOCK ) {
                return (-1);
            }
            UNCHECKED_WIN32C_ERROR(recv, error);
        }
        return (result);
    }

    void Stream::getall ( void * data, int size )
    {
        getall(static_cast<char*>(data), size);
    }

    void Stream::getall ( char * data, int size )
    {
        int used = 0;
        int pass = 0;
        do {
            pass = get(data+used, size-used);
        }
            while ((pass > 0) && ((used+=pass) < size));
    }

    int Stream::put ( const void * buffer, int length )
    {
        const int result = ::send
            (handle(), static_cast<const char*>(buffer), length, 0);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(shutdown, error);
        }
        return (result);
    }

    void Stream::putall ( const void * data, int size )
    {
        putall(static_cast<const char*>(data), size);
    }

    void Stream::putall ( const char * data, int size )
    {
        int used = 0;
        int pass = 0;
        do {
            pass = put(data+used, size-used);
        }
        while ((pass > 0) && ((used+=pass) < size));
    }

    void Stream::putall ( const std::string& message )
    {
        putall(message.data(), message.size());
    }

    void Stream::shutdown ()
    {
        const int status = ::shutdown(handle(), SD_BOTH);
        if ( status == SOCKET_ERROR ) {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(shutdown, error);
        }
    }

    void Stream::shutdowni ()
    {
        const int status = ::shutdown(handle(), SD_RECEIVE);
        if ( status == SOCKET_ERROR ) {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(shutdown, error);
        }
    }

    void Stream::shutdowno ()
    {
        const int status = ::shutdown(handle(), SD_SEND);
        if ( status == SOCKET_ERROR ) {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(shutdown, error);
        }
    }

} }
