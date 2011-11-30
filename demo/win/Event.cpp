// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Event.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Event.hpp"
#include "Error.hpp"
#include "Listener.hpp"
#include "Stream.hpp"

namespace win { namespace net {

    const Event::Mask Event::accept ()
    {
        return (FD_ACCEPT);
    }

    const Event::Mask Event::connect ()
    {
        return (FD_CONNECT);
    }

    const Event::Mask Event::close ()
    {
        return (FD_CLOSE);
    }

    const Event::Mask Event::get ()
    {
        return (FD_READ);
    }

    const Event::Mask Event::put ()
    {
        return (FD_WRITE);
    }

    Event::Event ( Mask mask )
        : myHandle(::WSACreateEvent())
    {
        if ( myHandle == 0 )
        {
            const int error = ::WSAGetLastError();
            // ...
        }
    }

    Event::~Event ()
    {
        ::WSACloseEvent(myHandle);
    }

    const Event::Handle Event::handle () const
    {
        return (myHandle);
    }

    Events::Events ( Stream& socket, Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        const int result =
            ::WSAEnumNetworkEvents(socket.handle(), event.handle(), &myData);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSAEnumNetworkEvents, error);
        }
    }

    Events::Events ( Listener& socket, Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        const int result =
            ::WSAEnumNetworkEvents(socket.handle(), event.handle(), &myData);
        if ( result == SOCKET_ERROR )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSAEnumNetworkEvents, error);
        }
    }

    bool Events::contains ( Event::Mask mask ) const
    {
        return (((myData.lNetworkEvents & mask) != 0)
            && (myData.iErrorCode[mask] == 0));
    }

} }
