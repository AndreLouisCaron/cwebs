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
#include <iostream>

namespace {

    ::DWORD wait
        ( const ::HANDLE * handles, ::DWORD count, ::BOOL all, ::DWORD timeout )
    {
        std::cout << "Waiting for " << count << " handles ";
        if (all) {
            std::cout << "(all)";
        }
        else {
            std::cout << "(any)";
        }
        std::cout << std::endl;
        for ( ::DWORD i = 0; (i < count); ++i )
        {
            std::cout
                << "  handle #" << i
                << ": " << handles[i] << std::endl;
        }
        const ::DWORD result = ::WaitForMultipleObjects(
            count, handles, all, timeout
            );
        std::cout << "WaitForMultipleObjects(): " << result << std::endl;
        if ( result == WAIT_FAILED )
        {
            std::cout << " ... failed!" << std::endl;
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(WaitForMultipleObjects, error);
        }
        if ( result == WAIT_TIMEOUT )
        {
            std::cout << " ... timeout!" << std::endl;
            return (MAXIMUM_WAIT_OBJECTS);
        }
        std::cout << " ... handle #" << (result-WAIT_OBJECT_0) << std::endl;
        return (result - WAIT_OBJECT_0);
    }

    ::DWORD wait ( const ::WSAEVENT * handles, ::DWORD count,
                   ::BOOL all, ::DWORD timeout, ::BOOL alertable )
    {
        std::cout << "Waiting for " << count << " handles ";
        if (all) {
            std::cout << "(all)";
        }
        else {
            std::cout << "(any)";
        }
        std::cout << std::endl;
        for ( ::DWORD i = 0; (i < count); ++i )
        {
            std::cout
                << "  handle #" << i
                << ": " << handles[i] << std::endl;
        }
        const ::DWORD result = ::WSAWaitForMultipleEvents(
            count, handles, all, timeout, alertable
            );
        std::cout << "WSAWaitForMultipleEvents(): " << result << std::endl;
        if ( result == WAIT_FAILED )
        {
            std::cout << " ... failed!" << std::endl;
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(WSAWaitForMultipleEvents, error);
        }
        if ( result == WAIT_TIMEOUT )
        {
            std::cout << " ... timeout!" << std::endl;
            return (MAXIMUM_WAIT_OBJECTS);
        }
        std::cout << " ... handle #" << (result-WAIT_OBJECT_0) << std::endl;
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
        std::cout << "WaitSet::add(HANDLE=" << value << ")" << std::endl;
            // Fixed capacity, sorry!
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
        std::cout << "WaitSet::add(WSAEVENT=" << value << ")" << std::endl;
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
