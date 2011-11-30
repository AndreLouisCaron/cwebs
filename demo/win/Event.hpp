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

namespace win { namespace net {

    class Stream;
    class Listener;

    class Event
    {
        /* nested types. */
    public:
        typedef long Mask;
        typedef ::WSAEVENT Handle;

        /* values. */
    public:
        static const Mask accept ();
        static const Mask connect ();
        static const Mask close ();
        static const Mask get ();
        static const Mask put ();
        static const Mask oob ();

        /* data. */
    private:
        Handle myHandle;

        /* construction. */
    public:
        Event ( Mask mask=0 );

    private:
        Event ( const Event& );

    public:
        ~Event ();

        /* methods. */
    public:
        const Handle handle () const;

    private:
        Event& operator= ( const Event& );
    };

    class Events
    {
        /* nested types.*/
    public:
        typedef ::WSANETWORKEVENTS Data;

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Events ( Stream& socket, Event& event );
        Events ( Listener& socket, Event& event );

        /* methods. */
    public:
        Data& data ();
        const Data& data () const;

        bool contains ( Event::Mask mask ) const;
    };

} }

#endif /* _win_Event_hpp__ */
