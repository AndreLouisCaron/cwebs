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
// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)

/*!
 * @file demo/nix/Tunnel.cpp
 */

#include "Tunnel.hpp"

#include "nix/WaitSet.hpp"

#include "Digest.hpp"

namespace {

    void tohost ( ::ws_iwire * stream, const void * data, uint64 size )
    {
        std::cout.write(static_cast<const char*>(data), size).flush();
    }

    void topeer ( ::ws_owire * stream, const void * data, uint64 size )
    {
        static_cast<nix::net::Stream*>(stream->baton)->putall(data, size);
    }

}

namespace nix {

    Tunnel::Tunnel ( nix::File& host, nix::net::Stream& peer )
        : myHost(host), myPeer(peer)
    {
        ::ws_iwire_init(&myIWire);
        myIWire.baton          = &myHost;
        myIWire.accept_content = &tohost;

        ::ws_owire_init(&myOWire);
        myOWire.baton          = &myPeer;
        myOWire.accept_content = &topeer;
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

    void Tunnel::exchange ( const std::string& host )
    {
        handshake(host);

        char data[1024];
        bool halive = true;
        bool palive = true;
        while (halive || palive)
        {
            // Wait for input from either end.
            nix::WaitSet streams;
            if (halive) {
                streams.add(myHost.handle());
            }
            if (palive) {
                streams.add(myPeer.handle());
            }
            nix::waitfori(streams);

            // Process host input.
            if (streams.contains(myHost.handle()))
            {
                const ssize_t size = myHost.get(data, sizeof(data));
                if ( size == 0 ) {
                    ::ws_owire_put_kill(&myOWire, 0, 0, 0);
                    myPeer.shutdowno();
                    halive = false;
                }
                else {
                    ::ws_owire_put_data(&myOWire, data, size, 0);
                }
            }

            // Process peer input.
            if (streams.contains(myPeer.handle()))
            {
                const ssize_t size = myPeer.get(data, sizeof(data));
                if ( size == 0 ) {
                    palive = false;
                }
                else {
                    ::ws_iwire_feed(&myIWire, data, size);
                }
            }
        }
    }
}
