// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/client-tunnel/client-tunnel.cpp
 * @brief Simple program that pipes standard streams to a websocket connection.
 *
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ws.hpp"
#include "Response.hpp"
#include "Digest.hpp"
#include "b64.hpp"

#include "win/Buffer.hpp"
#include "win/Context.hpp"
#include "win/Error.hpp"
#include "win/Endpoint.hpp"
#include "win/File.hpp"
#include "win/Stdin.hpp"
#include "win/Stream.hpp"
#include "win/Transfer.hpp"
#include "win/WaitSet.hpp"

namespace {

    void tohost ( ::ws_iwire * stream, const void * data, uint64 size )
    {
        std::cout.write(static_cast<const char*>(data), size).flush();
    }

    void topeer ( ::ws_owire * stream, const void * data, uint64 size )
    {
        static_cast<win::net::Stream*>(stream->baton)->putall(data, size);
    }

    std::string approve_nonce ( const std::string& skey )
    {
        static const std::string guid
            ("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        sha1::Digest digest;
        digest.update(skey.data(), skey.size());
        digest.update(guid.data(), guid.size());
        return (digest.result());
    }

    std::size_t handshake
        ( win::net::Stream& peer, char * data, std::size_t size )
    {
        // Generate a nonce.
        std::string nonce(16, '\0');
        { 
            char random[16];
            for ( int i =0 ; (i < 16); ++i) {
                random[i] = ::rand() % 255;
            }
            nonce.assign(random, 16);
        }
        nonce = b64::encode(nonce);
        
        // Send HTTP upgrade request.
        std::ostringstream request;
        request
            << "GET / HTTP/1.1"               << "\r\n"
            << "Host: ..."                    << "\r\n"
            << "Upgrade: websocket"           << "\r\n"
            << "Connection: upgrade"          << "\r\n"
            << "Sec-WebSocket-Key: " << nonce << "\r\n"
            << "Sec-WebSocket-Version: 13"    << "\r\n"
            << "\r\n";
        peer.putall(request.str());
        
        // Wait for upgrade approval.
        http::Response response;
        ::size_t used = 0;
        do {
            used = peer.get(data, size);
            if ( used == 0 ) {
                std::cerr << "Peer has finished." << std::endl;
                break;
            }
            used -= response.feed(data, used);
        }
        while ( !response.complete() );
        
        // Make sure we succeeded.
        if (!response.complete()) {
            std::cerr << "Did not finish HTTP response." << std::endl;
        }
        
        // Confirm handshake.
        if ((response.header("Connection") != "upgrade"  )||
            (response.header("Upgrade"   ) != "websocket"))
        {
            std::cerr << "Upgrade request denied." << std::endl;
        }
        const std::string version = response.header("Sec-WebSocket-Version");
        const std::string key = response.header("Sec-WebSocket-Accept");
        if (key != approve_nonce(nonce)) {
            std::cerr << "Invalid nonce reply." << std::endl;
        }
        
        // Keep any leftovers for the wire protocol.
        return (used);
    }

}

int main ( int argc, char ** argv )
try
{
    //win::File host("CONIN$");
    win::Stdin host;
    const win::net::Context context;
    win::net::Stream peer(
        win::net::Endpoint(127,0,0,1,8000));
    
    ::ws_owire owire;
    ::ws_owire_init(&owire);
    owire.baton          = &peer;
    owire.accept_content = &::topeer;
    ::ws_iwire iwire;
    ::ws_iwire_init(&iwire);
    iwire.baton          = &host;
    iwire.accept_content = &::tohost;

    char hdata[1024];
    char pdata[1024];
    win::net::Buffer pbuf(pdata, sizeof(pdata));

    // Perform WebSocket handshake.
    std::cout << "Starting hand-shake." << std::endl;
    ::ws_iwire_feed(&iwire, pdata,
        handshake(peer, pdata, sizeof(pdata)));

    std::cout << "Hand-shake complete." << std::endl;
    win::net::Event gate;
    win::net::Transfer xfer(gate);
    peer.get(pbuf, xfer);
    for ( bool running = true; running; )
    {
        // Wait for input from either end.
        win::net::WaitSet streams;
        streams.add(host.handle());
        streams.add(gate.handle());
        std::cout << "Waiting for input." << std::endl;
        const ::DWORD ready = win::net::any(streams);
        std::cout << "Ready! (handle #" << ready << ")" << std::endl;

        // Process host input.
        if (ready == 0)
        { 
            const int size = host.get(hdata, sizeof(hdata));
            std::cout << "Got " << size << " bytes from host." << std::endl;
            if ( size == 0 ) {
                ::ws_owire_put_kill(&owire, 0, 0);
                peer.shutdowno();
                running = false;
            }
            ::ws_owire_put_data(&owire, hdata, size);
        }

        // Process peer input.
        if (ready == 1)
        {
            ::DWORD size = 0;
            xfer.complete(peer, size);
            std::cout << "Got " << size << " bytes from peer." << std::endl;
            if ( size == 0 ) {
                running = false;
            }
            ::ws_iwire_feed(&iwire, pdata, size);
            // Start a new transfer.
            gate.reset();
            xfer.reset(gate);
            peer.get(pbuf, xfer);
        }
    }
}
catch ( const win::Error& error )
{
    std::cerr
      << "Windows error: '" << error.what() << "'."
      << std::endl;
    return (EXIT_FAILURE);
}
catch ( const std::exception& error )
{
    std::cerr
      << "Uncaught exception: '" << error.what() << "'."
      << std::endl;
    return (EXIT_FAILURE);
}
catch ( ... )
{
    std::cerr
        << "Uncaught exception of unknown type."
        << std::endl;
    return (EXIT_FAILURE);
}
