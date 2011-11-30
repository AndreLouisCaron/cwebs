// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

#include "Context.hpp"
#include "Error.hpp"

namespace win { namespace net {

    Context::Context ()
    {
            // Clear the structure, just in case.
        ::ZeroMemory(&myData, sizeof(myData));

            // Initialize library.
        const int result = ::WSAStartup(MAKEWORD(2,2), &myData);
        if ( result != 0 ) {
            UNCHECKED_WIN32C_ERROR(WSAStartup,result);
        }
    }

    Context::~Context ()
    {
        const int result = ::WSACleanup();
        if ( result != 0 ) {
            UNCHECKED_WIN32C_ERROR(WSACleanup,result);
        }
    }

    const Context::Data& Context::data () const
    {
        return (myData);
    }

} }
