// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Thread.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Thread.hpp"
#include "Error.hpp"

namespace {

    ::HANDLE allocate ( ::LPTHREAD_START_ROUTINE function, ::LPVOID parameter )
    {
        ::DWORD identifier = 0;
        const ::HANDLE result = ::CreateThread(
            0, 0, function, parameter, 0, &identifier
            );
        if ( result == INVALID_HANDLE_VALUE )
        {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(CreateThread, error);
        }
        return (result);
    }

}

namespace win {

    Thread::Thread ( Function function, void * context )
        : myHandle(::allocate(function, context))
    {
    }

    Thread::~Thread ()
    {
        const ::BOOL result = ::CloseHandle(myHandle);
        if ( result == FALSE )
        {
            const ::DWORD error = ::GetLastError();
            //UNCHECKED_WIN32C_ERROR(CloseThread, error);
        }
    }

    Thread::Handle Thread::handle () const
    {
        return (myHandle);
    }

    Thread::Identifier Thread::identifier () const
    {
        const ::DWORD identifier = ::GetThreadId(handle());
        if ( identifier == 0 ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(GetThreadId, error);
        }
        return (identifier);
    }

    void Thread::join () const
    {
        const ::DWORD result = ::WaitForSingleObject(handle(), INFINITE);
        if ( result != WAIT_OBJECT_0 ) {
            const ::DWORD error = ::GetLastError();
            UNCHECKED_WIN32C_ERROR(GetThreadId, error);
        }
    }

}
