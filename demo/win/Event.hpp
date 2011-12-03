#ifndef _win_Event_hpp__
#define _win_Event_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Event.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win {

    class Event
    {
        /* nested types. */
    public:
        typedef ::HANDLE Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Event ();

    private:
        Event ( const Event& );

    public:
        ~Event ();

        /* methods. */
    public:
        const Handle handle () const;

        void set ();
        void reset ();

    private:
        Event& operator= ( const Event& );
    };

}

namespace win { namespace net {

    class Event
    {
        /* nested types. */
    public:
        typedef ::WSAEVENT Handle;

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Event ();

    private:
        Event ( const Event& );

    public:
        ~Event ();

        /* methods. */
    public:
        const Handle handle () const;

        void set ();
        void reset ();

    private:
        Event& operator= ( const Event& );
    };

} }

#endif /* _win_Event_hpp__ */
