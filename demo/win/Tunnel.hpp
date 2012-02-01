#ifndef _win_Tunnel_hpp__
#define _win_Tunnel_hpp__

// Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*!
 * @file demo/win/Tunnel.hpp
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
