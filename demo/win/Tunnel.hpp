#ifndef _win_Tunnel_hpp__
#define _win_Tunnel_hpp__

// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Tunnel.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "webs.h"
#include "mt19937.h"
#include "win/Stdin.hpp"
#include "win/Stdout.hpp"
#include "win/Stream.hpp"

#include <string>

namespace win {

    class Tunnel
    {
        /* data. */
    private:
        ::mt19937_prng myPrng;

    protected:
        win::Stdout myHostO;
        win::Stdin& myHostI;
        win::net::Stream& myPeer;

        ::ws_iwire myIWire;
        ::ws_owire myOWire;

    protected:
        Tunnel ( win::Stdin& host, win::net::Stream& peer, uint32_t salt=0 );

        /* methods. */
    protected:
        static std::string approve_nonce ( const std::string& key );
        void generate_nonce ( void * data, size_t size );

        virtual void handshake ( const std::string& host ) = 0;

    public:
        void exchange ( const std::string& host );

    private:
        void foreground ();
        void background ();

        static void background ( void * context );

        static void tohost
            ( ::ws_iwire * stream, const void * data, uint64 size );
        static void topeer
            ( ::ws_owire * stream, const void * data, uint64 size );
    };

}

#endif /* _win_Tunnel_hpp__ */
