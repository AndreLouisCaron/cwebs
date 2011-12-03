// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Tunnel.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Tunnel.hpp"

#include "win/Buffer.hpp"
#include "win/Thread.hpp"
#include "win/Transfer.hpp"
#include "win/WaitSet.hpp"

#include "Digest.hpp"

namespace {

    void tohost ( ::ws_iwire * stream, const void * data, uint64 size )
    {
        std::cout.write(static_cast<const char*>(data), size).flush();
    }

    void topeer ( ::ws_owire * stream, const void * data, uint64 size )
    {
        static_cast<win::net::Stream*>(stream->baton)->putall(data, size);
    }

}

namespace win {

    Tunnel::Tunnel ( win::Stdin& host, win::net::Stream& peer )
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

    void Tunnel::exchange ()
    {
        handshake();

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
        char data[1024];

        // Tunnel all host input through the connection.
        for ( int size = 0; ((size=myHost.get(data, sizeof(data))) > 0); )
        {
            ::ws_owire_put_data(&myOWire, data, size);
        }

        // Let the peer know we won't be sending any more data.
        ::ws_owire_put_kill(&myOWire, 0, 0);
        myPeer.shutdowno();

        // Let the background thread know we're done and
        // wait for interaction with the peer complete.
        myDone.set();
    }

    void Tunnel::background ( void * context )
    {
        static_cast<Tunnel*>(context)->background();
    }

    void Tunnel::background ()
    {
        bool halive = true;
        bool palive = true;

        // Tools for asynchronous tranfers.
        char data[1024];
        win::net::Buffer pbuf(data, sizeof(data));
        win::net::Event gate;
        win::net::Transfer xfer(gate);

        while ( halive && palive )
        {
            // Start an asynchronous transfer.
            gate.reset();
            xfer.reset(gate);
            myPeer.get(pbuf, xfer);

            // Expect either asynchronous transfer completion
            // or notification that host input is exhausted.
            win::net::WaitSet conditions;
            conditions.add(myDone.handle());
            conditions.add(  gate.handle());
            const ::DWORD which = win::net::any(conditions);

            // Host input exhausted.
            if (which == 0) {
                halive = false;
            }

            // Peer input received.
            if (which == 1)
            {
                ::DWORD size = 0;
                xfer.complete(myPeer, size);
                if ( size == 0 ) {
                    palive = false;
                }
                ::ws_iwire_feed(&myIWire, data, size);
            }
        }

        while ( palive )
        {
            // Start an asynchronous transfer.
            gate.reset();
            xfer.reset(gate);
            myPeer.get(pbuf, xfer);

            // Wait for input from peer.
            win::net::WaitSet conditions;
            conditions.add(gate.handle());
            const ::DWORD which = win::net::any(conditions);

            // Process received data.
            ::DWORD size = 0;
            xfer.complete(myPeer, size);
            if ( size == 0 ) {
                palive = false;
            }
            ::ws_iwire_feed(&myIWire, data, size);
        }

        myPeer.shutdowni();
    }

}
