#ifndef _win_Context_hpp__
#define _win_Context_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

#include <WinSock2.h>
#include <Windows.h>

namespace win { namespace net {

    class Context
    {
        /* nested types. */
    public:
        typedef ::WSADATA Data;

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Context ();

    private:
        Context ( const Context& );

    public:
        ~Context ();

        /* methods. */
    public:
        const Data& data () const;

    private:
        Context& operator= ( const Context& );
    };

} }

#endif /* _win_Context_hpp__ */
