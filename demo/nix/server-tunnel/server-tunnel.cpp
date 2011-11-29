/*!
 * @file demo/nix/tunnel/server-tunnel.cpp
 * @brief Simple program that pipes a websocket connection to a program.
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ws.hpp"
#include "Request.hpp"
#include "Digest.hpp"
#include "b64.hpp"


#include "Endpoint.hpp"
#include "File.hpp"
#include "Listener.hpp"
#include "Stream.hpp"
#include "WaitSet.hpp"

namespace {

    void tohost ( ::ws_iwire * stream, const void * data, uint64 size )
    {
        std::cout.write(static_cast<const char*>(data), size).flush();
    }

    void topeer ( ::ws_owire * stream, const void * data, uint64 size )
    {
        static_cast<nix::net::Stream*>(stream->baton)->putall(data, size);
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
        ( nix::net::Stream& peer, char * data, std::size_t size )
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
    nix::File host(STDIN_FILENO);
    nix::net::Listener listener(
        nix::net::Endpoint(127,0,0,1,8000));
    nix::net::Stream peer(listener);
    
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
        nix::WaitSet streams;
        streams.add(host.handle());
        streams.add(peer.handle());
        const int ready = nix::waitfori(streams);
        
        // Process host input.
        if (streams.contains(host.handle()))
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
        if (streams.contains(peer.handle()))
        {
            const ssize_t size = peer.get(data, sizeof(data));
            if ( size == 0 ) {
                running = false;
            }
            ::ws_iwire_feed(&iwire, data, size);
        }
    }
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
