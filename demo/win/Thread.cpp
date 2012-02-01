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
 * @file demo/win/Thread.cpp
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
