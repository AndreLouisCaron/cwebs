// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/WaitSet.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "WaitSet.hpp"
#include "Error.hpp"
#include <algorithm>

namespace {

    ::DWORD wait
        ( const ::HANDLE * handles, ::DWORD count, ::BOOL all, ::DWORD timeout )
    {
        const ::DWORD result = ::WaitForMultipleObjects
            (count, handles, all, timeout);
        if ( result == WAIT_FAILED )
        {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(WaitForMultipleObjects, error);
        }
        if ( result == WAIT_TIMEOUT )
        {
            return (MAXIMUM_WAIT_OBJECTS);
        }
        return (result - WAIT_OBJECT_0);
    }

    ::DWORD wait ( const ::WSAEVENT * handles, ::DWORD count,
                   ::BOOL all, ::DWORD timeout, ::BOOL alertable )
    {
        const ::DWORD result = ::WSAWaitForMultipleEvents
            (count, handles, all, timeout, alertable);
        if ( result == WAIT_FAILED )
        {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(WSAWaitForMultipleEvents, error);
        }
        if ( result == WAIT_TIMEOUT )
        {
            return (MAXIMUM_WAIT_OBJECTS);
        }
        return (result - WAIT_OBJECT_0);
    }

}

namespace win {

    WaitSet::WaitSet ()
    {
        ::ZeroMemory(myData, sizeof(myData)), mySize = 0;
    }

    ::DWORD WaitSet::capacity ()
    {
        return (MAXIMUM_WAIT_OBJECTS);
    }

    ::DWORD WaitSet::size () const
    {
        return (mySize);
    }

    const ::HANDLE * WaitSet::data () const
    {
        return (myData);
    }

    WaitSet& WaitSet::add ( ::HANDLE value )
    {
        if (size() < capacity())
        {
            if ( std::find(begin(), end(), value) == end() ) {
                *end() = value, ++mySize;
            }
        }
        return (*this);
    }

    WaitSet& WaitSet::del ( ::HANDLE value )
    {
        const iterator match = std::find(begin(), end(), value);
        if ( match != end() ) {
            std::copy(match+1, end(), match), --mySize;
        }
        return (*this);
    }

    bool WaitSet::contains ( ::HANDLE value ) const
    {
        return (std::find(begin(), end(), value) != end());
    }

    ::DWORD any ( const WaitSet& set )
    {
        return (::wait(set.data(), set.size(), FALSE, INFINITE));
    }

    ::DWORD any ( const WaitSet& set, ::DWORD timeout )
    {
        return (::wait(set.data(), set.size(), FALSE, timeout));
    }

    ::DWORD all ( const WaitSet& set )
    {
        return (::wait(set.data(), set.size(), TRUE, INFINITE));
    }

    ::DWORD all ( const WaitSet& set, ::DWORD timeout )
    {
        return (::wait(set.data(), set.size(), TRUE, timeout));
    }

}

namespace win { namespace net {

    WaitSet::WaitSet ()
    {
        ::ZeroMemory(myData, sizeof(myData)), mySize = 0;
    }

    ::DWORD WaitSet::capacity ()
    {
        return (MAXIMUM_WAIT_OBJECTS);
    }

    ::DWORD WaitSet::size () const
    {
        return (mySize);
    }

    const ::WSAEVENT * WaitSet::data () const
    {
        return (myData);
    }

    WaitSet& WaitSet::add ( ::WSAEVENT value )
    {
        if (size() < capacity())
        {
            if ( std::find(begin(), end(), value) == end() ) {
                *end() = value, ++mySize;
            }
        }
        return (*this);
    }

    WaitSet& WaitSet::del ( ::WSAEVENT value )
    {
        const iterator match = std::find(begin(), end(), value);
        if ( match != end() ) {
            std::copy(match+1, end(), match), --mySize;
        }
        return (*this);
    }

    bool WaitSet::contains ( ::WSAEVENT value ) const
    {
        return (std::find(begin(), end(), value) != end());
    }

    ::DWORD any ( const WaitSet& set )
    {
        return (::wait(set.data(), set.size(), FALSE, WSA_INFINITE, FALSE));
    }

    ::DWORD any ( const WaitSet& set, ::DWORD timeout )
    {
        return (::wait(set.data(), set.size(), FALSE, timeout, FALSE));
    }

    ::DWORD all ( const WaitSet& set )
    {
        return (::wait(set.data(), set.size(), TRUE, WSA_INFINITE));
    }

    ::DWORD all ( const WaitSet& set, ::DWORD timeout )
    {
        return (::wait(set.data(), set.size(), TRUE, timeout, FALSE));
    }

}  }
