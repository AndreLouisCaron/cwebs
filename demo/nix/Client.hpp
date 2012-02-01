#ifndef _nix_Client_hpp__
#define _nix_Client_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/nix/Client.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Tunnel.hpp"

namespace nix {

    class Client :
        public Tunnel
    {
        /* constrution. */
    public:
        Client ( nix::File& host, nix::net::Stream& peer );

        /* methods. */
    private:
        std::size_t handshake
            ( const std::string& host, char * data, std::size_t size );

        /* overrides. */
    protected:
        virtual void handshake ( const std::string& host );
    };

}

#endif /* _nix_Client_hpp__ */
