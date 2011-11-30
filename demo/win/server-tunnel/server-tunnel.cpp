// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/server-tunnel/client-tunnel.cpp
 * @brief Simple program that pipes a websocket connection to a program.
 *
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ws.hpp"
#include "Request.hpp"
#include "Digest.hpp"
#include "b64.hpp"


#include "win/Context.hpp"
#include "win/Error.hpp"
#include "win/Endpoint.hpp"
#include "win/File.hpp"
#include "win/Listener.hpp"
#include "win/Stream.hpp"
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
        http::Request request;
        ::size_t used = 0;
        do {
            used = peer.get(data, size);
            if ( used == 0 ) {
                std::cerr << "Peer has finished." << std::endl;
                break;
            }
            used -= request.feed(data, used);
        }
        while ( !request.complete() );
        
        // Make sure we succeeded.
        if (!request.complete()) {
            std::cerr << "Did not finish HTTP request." << std::endl;
        }
        
        // Confirm handshake.
        if ((request.header("Connection") != "upgrade"  )||
            (request.header("Upgrade"   ) != "websocket"))
        {
            std::cerr << "Invalid upgrade request." << std::endl;
        }
        const std::string version = request.header("Sec-WebSocket-Version");
        if (version != "13") {
            std::cerr << "Incompatible versions." << std::endl;
        }
        std::string nonce = request.header("Sec-WebSocket-Key");
        if (nonce.empty()) {
            std::cerr << "Empty nonce." << std::endl;
        }
        std::string key = approve_nonce(nonce);
        
        // Send HTTP upgrade approval.
        std::ostringstream response;
        response
            << "HTTP/1.1 101 Switching Protocols" << "\r\n"
            << "Upgrade: websocket"               << "\r\n"
            << "Connection: upgrade"              << "\r\n"
            << "Sec-WebSocket-Accept: " << key    << "\r\n"
            << "Sec-WebSocket-Version: 13"        << "\r\n"
            << "\r\n";
        peer.putall(response.str());
        
        // Keep any leftovers for the wire protocol.
        return (used);
    }

}

int main ( int argc, char ** argv )
try
{
    win::File host("CONIN$");
    const win::net::Context context;
    win::net::Listener listener(
        win::net::Endpoint(127,0,0,1,8000));
    win::net::Stream peer(listener);
    
    ::ws_owire owire;
    ::ws_owire_init(&owire);
    owire.baton          = &peer;
    owire.accept_content = &::topeer;
    ::ws_iwire iwire;
    ::ws_iwire_init(&iwire);
    iwire.baton          = &host;
    iwire.accept_content = &::tohost;
    
    char data[1024];
    
    // Perform WebSocket handshake.
    ::ws_iwire_feed(&iwire, data,
        handshake(peer, data, sizeof(data)));
    
    for ( bool running = true; running; )
    {
        // Wait for input from either end.
        win::WaitSet streams;
        streams.add(host.handle());
        streams.add(peer.handle());
        const ::DWORD ready = win::any(streams);

        // Check for errors.
        if (ready == win::WaitSet::capacity())
        {
            std::cerr << "Could not wait for handles!" << std::endl;
            return (EXIT_FAILURE);
        }

        // Process host input.
        if (ready == 0)
        {
            const ssize_t size = host.get(data, sizeof(data));
            if ( size == 0 ) {
                ::ws_owire_put_kill(&owire, 0, 0);
                peer.shutdowno();
                running = false;
            }
            ::ws_owire_put_data(&owire, data, size);
        }
        
        // Process peer input.
        if (ready == 1)
        {
            const ssize_t size = peer.get(data, sizeof(data));
            if ( size == 0 ) {
                running = false;
            }
            ::ws_iwire_feed(&iwire, data, size);
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
