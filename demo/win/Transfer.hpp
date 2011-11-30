#ifndef _win_Transfer_hpp__
#define _win_Transfer_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Transfer.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win { namespace net {

    class Event;
    class Stream;

    class Transfer
    {
        /* nested types. */
    public:
        typedef ::WSAOVERLAPPED Data;

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Transfer ( Event& event );

    private:
        Transfer ( const Transfer& );

    public:
        ~Transfer ();

        /* methods. */
    public:
        Data& data ();

        void reset ( Event& event );

        void complete ( Stream& stream, ::DWORD& transferred );
        void complete ( Stream& stream, ::DWORD& transferred, ::DWORD& status );

        bool result ( Stream& stream, ::DWORD& transferred );
        bool result ( Stream& stream, ::DWORD& transferred, ::DWORD& status );

        /* operators. */
    private:
        Transfer& operator= ( const Transfer& );
    };

} }

#endif /* _win_Transfer_hpp__ */
