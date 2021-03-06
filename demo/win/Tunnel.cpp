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
 * @file demo/win/Tunnel.cpp
 */

#include "Tunnel.hpp"
#include "win/Thread.hpp"
#include "Digest.hpp"

#include <ctime>
#include <iostream>

namespace win {

    Tunnel::Tunnel ( win::Stdin& host, win::net::Stream& peer, uint32_t salt )
        : myHostI(host), myPeer(peer)
    {
        ::ws_iwire_init(&myIWire);
        myIWire.baton          = this;
        myIWire.accept_content = &tohost;

        ::ws_owire_init(&myOWire);
        myOWire.baton          = this;
        myOWire.accept_content = &topeer;

        // Use a real pseudo-random number generator for masks and nonces.
        ::mt19937_prng_init(&myPrng, uint32_t(::time(0)^salt));
        myOWire.rand = &domask;
    }

    std::string Tunnel::approve_nonce ( const std::string& skey )
    {
        static const std::string guid
            ("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        sha1::Digest digest;
        digest.update(skey.data(), skey.size());
        digest.update(guid.data(), guid.size());
        return (digest.result());
    }

    void Tunnel::generate_nonce ( void * data, size_t size )
    {
        ::mt19937_prng_grab(&myPrng, data, size);
    }

    void Tunnel::exchange ( const std::string& host )
    {
        handshake(host);

        // Run interaction with peer in background thread because
        // we can't do anything but block on standard input (we
        // don't know if it supports asynchronous or non-blocking
        // I/O or even if we can reliably wait on the handle for
        // input notifications.
        win::Thread thread(
            win::Thread::adapt<void,void*,&Tunnel::background>(), this);
        foreground();
        thread.join();
    }

    void Tunnel::foreground ()
    {
        // Tools for synchronous transfer.
        char data[4*1024];

        // Tunnel all host input through the connection.
        for ( int size = 0; ((size=myHostI.get(data, sizeof(data))) > 0); )
        {
            ::ws_owire_put_data(&myOWire, data, size, 0);
        }

        // Let the peer know we won't be sending any more data.
        ::ws_owire_put_kill(&myOWire, 0, 0, 0);
        myPeer.shutdowno();
    }

    void Tunnel::background ( void * context )
    {
        static_cast<Tunnel*>(context)->background();
    }

    void Tunnel::background ()
    {
        // Tools for synchronous transfer.
        char data[1024];

        // Tunnel all host input through the connection.
        for ( int size = 0; ((size=myPeer.get(data, sizeof(data))) > 0); )
        {
            ::ws_iwire_feed(&myIWire, data, size);
            if (myIWire.status != ::ws_iwire_ok)
            {
                std::cerr
                    << "WebSocket parser error: '";
                if (myIWire.status == ws_iwire_invalid_extension)
                {
                    std::cerr << "invalid extension";
                }
                if (myIWire.status == ws_iwire_unknown_message_type)
                {
                    std::cerr << "unknown message type";
                }
                if (myIWire.status == ws_iwire_message_type_changed)
                {
                    std::cerr << "message type changed";
                }
                if (myIWire.status == ws_iwire_masking_required)
                {
                    std::cerr << "masking required";
                }
                std::cerr
                    << "'."
                    << std::endl;
                break;
            }
        }

        // Let peer know we're not expecting any more data.
        myPeer.shutdowni();
    }

    void Tunnel::domask ( struct ws_owire * stream, uint8 mask[4] )
    {
        Tunnel& tunnel = *static_cast<Tunnel*>(stream->baton);
        ::mt19937_prng_grab(&tunnel.myPrng, mask, 4);
    }

    void Tunnel::tohost ( ::ws_iwire * stream, const void * data, uint64 size )
    {
        Tunnel& tunnel = *static_cast<Tunnel*>(stream->baton);
        uint64 ubnd = static_cast<uint64>(MAXDWORD);
        uint64 used = 0;
        while (used < size) {
            const ::DWORD pass = static_cast<::DWORD>(MIN(ubnd, size));
            tunnel.myHostO.putall(static_cast<const char*>(data)+used, pass);
            used += pass;
        }
        tunnel.myHostO.flush();
    }

    void Tunnel::topeer ( ::ws_owire * stream, const void * data, uint64 size )
    {
        Tunnel& tunnel = *static_cast<Tunnel*>(stream->baton);
        uint64 ubnd = static_cast<uint64>(MAXDWORD);
        uint64 used = 0;
        while (used < size) {
            const ::DWORD pass = static_cast<::DWORD>(MIN(ubnd, size));
            tunnel.myPeer.putall(static_cast<const char*>(data)+used, pass);
            used += pass;
        }
    }

}
