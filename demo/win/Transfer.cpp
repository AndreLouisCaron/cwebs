// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Transfer.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Transfer.hpp"
#include "Error.hpp"
#include "Event.hpp"
#include "File.hpp"
#include "Stdin.hpp"
#include "Stream.hpp"

namespace {

    bool poll ( ::HANDLE stream, ::LPOVERLAPPED transfer,
                ::BOOL wait, ::LPDWORD transferred )
    {
        const ::BOOL result = ::GetOverlappedResult
            (stream, transfer, transferred, wait);
        if ( result == FALSE )
        {
            const int error = ::WSAGetLastError();
            if ( error == WSA_IO_INCOMPLETE ) {
                return (false);
            }
            UNCHECKED_WIN32C_ERROR(GetOverlappedResult, error);
        }
        return (true);
    }

    bool poll ( ::SOCKET stream, ::LPWSAOVERLAPPED transfer,
                ::BOOL wait, ::LPDWORD transferred, ::LPDWORD status )
    {
        const ::BOOL result = ::WSAGetOverlappedResult
            (stream, transfer, transferred, wait, status);
        if ( result == FALSE )
        {
            const int error = ::WSAGetLastError();
            if ( error == WSA_IO_INCOMPLETE ) {
                return (false);
            }
            UNCHECKED_WIN32C_ERROR(WSAGetOverlappedResult, error);
        }
        return (true);
    }

}

namespace win {

    Transfer::Transfer ( Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.hEvent = event.handle();
    }

    Transfer::~Transfer ()
    {
        ::ZeroMemory(&myData, sizeof(myData));
    }

    Transfer::Data& Transfer::data ()
    {
        return (myData);
    }

    void Transfer::reset ( Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.hEvent = event.handle();
    }

    void Transfer::complete
        ( File& stream, ::DWORD& transferred )
    {
        ::poll(stream.handle(), &myData, TRUE, &(transferred=0));
    }

    bool Transfer::result ( File& stream, ::DWORD& transferred )
    {
        return (::poll(stream.handle(), &myData, FALSE, &(transferred=0)));
    }

    void Transfer::complete ( Stdin& stream, ::DWORD& transferred )
    {
        ::poll(stream.handle(), &myData, TRUE, &(transferred=0));
    }

    bool Transfer::result ( Stdin& stream, ::DWORD& transferred )
    {
        return (::poll(stream.handle(), &myData, FALSE, &(transferred=0)));
    }

}

namespace win { namespace net {

    Transfer::Transfer ( Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.hEvent = event.handle();
    }

    Transfer::~Transfer ()
    {
        ::ZeroMemory(&myData, sizeof(myData));
    }

    Transfer::Data& Transfer::data ()
    {
        return (myData);
    }

    void Transfer::reset ( Event& event )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.hEvent = event.handle();
    }

    void Transfer::complete ( Stream& stream, ::DWORD& transferred )
    {
        ::DWORD status = 0;
        complete(stream, transferred, status);
    }

    void Transfer::complete
        ( Stream& stream, ::DWORD& transferred, ::DWORD& status )
    {
        ::poll(stream.handle(), &myData, TRUE,
               &(transferred=0), &(status=0));
    }

    bool Transfer::result ( Stream& stream, ::DWORD& transferred )
    {
        ::DWORD status = 0;
        return (result(stream, transferred, status));
    }

    bool Transfer::result
        ( Stream& stream, ::DWORD& transferred, ::DWORD& status )
    {
        return (::poll(stream.handle(), &myData, FALSE,
                       &(transferred=0), &(status=0)));
    }

} }
