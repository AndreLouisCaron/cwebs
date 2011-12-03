#ifndef _nix_Tunnel_hpp__
#define _nix_Tunnel_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/nix/Tunnel.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "ws.hpp"
#include "nix/File.hpp"
#include "nix/Stream.hpp"

#include <string>

namespace nix {

    class Tunnel
    {
        /* data. */
    protected:
        nix::File& myHost;
        nix::net::Stream& myPeer;

        ::ws_iwire myIWire;
        ::ws_owire myOWire;

    protected:
        Tunnel ( nix::File& host, nix::net::Stream& peer );

        /* methods. */
    protected:
        static std::string approve_nonce ( const std::string& key );

        virtual void handshake () = 0;

    public:
        void exchange ();

    private:
        void foreground ();
        void background ();

        static void background ( void * context );
    };

}

#endif /* _nix_Tunnel_hpp__ */
