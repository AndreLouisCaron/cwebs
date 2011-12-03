// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Stream.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Stream.hpp"
#include "Buffer.hpp"
#include "Endpoint.hpp"
#include "Error.hpp"
#include "Listener.hpp"
#include "Transfer.hpp"
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

    void Stream::select ( Event& event, long mask )
    {
        const int result = ::WSAEventSelect(handle(), event.handle(), mask);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSAEventSelect, error);
        }
    }

    void Stream::get ( Buffer& buffer, Transfer& transfer )
    {
        ::DWORD flags = 0;
        const int result = ::WSARecv
            (handle(), &buffer.data(), 1, 0, &flags, &transfer.data(), 0);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            if ( error != WSA_IO_PENDING ) {
                UNCHECKED_WIN32C_ERROR(WSARecv, error);
            }
        }
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
