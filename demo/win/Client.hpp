#ifndef _win_Client_hpp__
#define _win_Client_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Client.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Tunnel.hpp"

namespace win {

    class Client :
        public Tunnel
    {
        /* constrution. */
    public:
        Client ( win::Stdin& host, win::net::Stream& peer );

        /* methods. */
    private:
        std::size_t handshake
            ( win::net::Stream& peer, char * data, std::size_t size );

        /* overrides. */
    protected:
        virtual void handshake ();
    };

}

#endif /* _win_Client_hpp__ */