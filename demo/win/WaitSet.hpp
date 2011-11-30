#ifndef _win_WaitSet_hpp__
#define _win_WaitSet_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/WaitSet.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>

namespace win {

    class WaitSet
    {
        /* nested types. */
    public:
        typedef ::HANDLE * iterator;
        typedef const ::HANDLE * const_iterator;

        /* data. */
    private:
        ::HANDLE myData[MAXIMUM_WAIT_OBJECTS];
        ::DWORD mySize;

        /* construction. */
    public:
        WaitSet ();

        /* methods. */
    public:
        static ::DWORD capacity ();

        ::DWORD size () const;

        const ::HANDLE * data () const;

        const_iterator begin () const { return (myData); }
        const_iterator end () const { return (begin()+size()); }

        bool empty () const { return (size() == 0); }
        bool full () const { return (size() == capacity()); }

        WaitSet& add ( ::HANDLE value );
        WaitSet& del ( ::HANDLE value );
        bool contains ( ::HANDLE value ) const;

    private:
        iterator begin () { return (myData); }
        iterator end () { return (begin()+size()); }
    };

    ::DWORD any ( const WaitSet& set );
    ::DWORD any ( const WaitSet& set, ::DWORD timeout );
    ::DWORD all ( const WaitSet& set );
    ::DWORD all ( const WaitSet& set, ::DWORD timeout );

}

namespace win { namespace net {

    class WaitSet
    {
        /* nested types. */
    public:
        typedef ::WSAEVENT * iterator;
        typedef const ::WSAEVENT * const_iterator;

        /* data. */
    private:
        ::WSAEVENT myData[MAXIMUM_WAIT_OBJECTS];
        ::DWORD mySize;

        /* construction. */
    public:
        WaitSet ();

        /* methods. */
    public:
        static ::DWORD capacity ();

        ::DWORD size () const;

        const ::WSAEVENT * data () const;

        const_iterator begin () const { return (myData); }
        const_iterator end () const { return (begin()+size()); }

        bool empty () const { return (size() == 0); }
        bool full () const { return (size() == capacity()); }

        WaitSet& add ( ::WSAEVENT value );
        WaitSet& del ( ::WSAEVENT value );
        bool contains ( ::WSAEVENT value ) const;

    private:
        iterator begin () { return (myData); }
        iterator end () { return (begin()+size()); }
    };

    ::DWORD any ( const WaitSet& set );
    ::DWORD any ( const WaitSet& set, ::DWORD timeout );
    ::DWORD all ( const WaitSet& set );
    ::DWORD all ( const WaitSet& set, ::DWORD timeout );

} }

#endif /* _win_WaitSet_hpp__ */
