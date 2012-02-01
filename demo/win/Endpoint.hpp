#ifndef _win_Endpoint_hpp__
#define _win_Endpoint_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Endpoint.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <WinSock2.h>
#include <Windows.h>
#include <iosfwd>

namespace win { namespace net {

    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;

    class Endpoint
    {
        /* nested types. */
    public:
        typedef ::sockaddr_in Data;

        /* class methods. */
    public:
        static const Endpoint resolve ( const char * name, uint16_t port );

        static const Endpoint any ( uint16_t port );
        static const Endpoint localhost ( uint16_t port );

        /* data. */
    private:
        Data myData;

        /* construction. */
    public:
        Endpoint ();
        Endpoint ( const Data& data );
        Endpoint ( const Data& data, uint16_t port );
        Endpoint ( uint32_t address, uint16_t port );
        Endpoint
            ( uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint16_t port );

        Endpoint ( const Endpoint& other );

        /* methods. */
    public:
        int size () const;

        ::sockaddr * raw ();
        const ::sockaddr * raw () const;

        Data& data ();
        const Data& data () const;

        uint16_t port () const;
    };

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint );

} }

#endif /* _win_Endpoint_hpp__ */
