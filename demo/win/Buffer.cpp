// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Buffer.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Buffer.hpp"

namespace win { namespace net {

    Buffer::Buffer ( void * data, ::u_long size )
    {
        ::ZeroMemory(&myData, sizeof(myData));
        myData.len = size;
        myData.buf = reinterpret_cast<char*>(data);
    }

    Buffer::Data& Buffer::data ()
    {
        return (myData);
    }

} }
