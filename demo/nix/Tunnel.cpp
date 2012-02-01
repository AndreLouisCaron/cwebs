// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/nix/Tunnel.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
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
                    ::ws_owire_put_kill(&myOWire, 0, 0);
                    myPeer.shutdowno();
                    halive = false;
                }
                else {
                    ::ws_owire_put_data(&myOWire, data, size);
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
