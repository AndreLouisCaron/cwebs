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

namespace win {

    Event::Event ()
        : myHandle(::CreateEvent(0,TRUE,FALSE,0))
    {
        if ( myHandle == 0 )
        {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(CreateEvent, error);
        }
    }

    Event::~Event ()
    {
        ::CloseHandle(myHandle);
    }

    const Event::Handle Event::handle () const
    {
        return (myHandle);
    }

    void Event::set ()
    {
        const ::BOOL result = ::SetEvent(handle());
        if ( result == FALSE )
        {
            const ::DWORD error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(SetEvent, error);
        }
    }

    void Event::reset ()
    {
        const ::BOOL result = ::ResetEvent(handle());
        if ( result == FALSE )
        {
            const ::DWORD error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(ResetEvent, error);
        }
    }

}

namespace win { namespace net {

    Event::Event ()
        : myHandle(::WSACreateEvent())
    {
        if ( myHandle == 0 )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSACreateEvent, error);
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

    void Event::set ()
    {
        const ::BOOL result = ::WSASetEvent(handle());
        if ( result == FALSE )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSASetEvent, error);
        }
    }

    void Event::reset ()
    {
        const ::BOOL result = ::WSAResetEvent(handle());
        if ( result == FALSE )
        {
            const int error = ::WSAGetLastError();
            UNCHECKED_WIN32C_ERROR(WSAResetEvent, error);
        }
    }

} }
