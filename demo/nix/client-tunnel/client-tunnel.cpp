/*!
 * @file demo/nix/tunnel/client-tunnel.cpp
 * @brief Simple program that pipes standard streams to a websocket connection.
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ws.hpp"
#include "Response.hpp"
#include "Digest.hpp"
#include "b64.hpp"

#include "Endpoint.hpp"
#include "File.hpp"
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
        // Generate a nonce.
        std::string nonce(16, '\0');
        { 
            nix::File stream("/dev/urandom");
            char random[16];
            stream.getall(random, 16);
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
    nix::File host(STDIN_FILENO);
    nix::net::Stream peer(
        nix::net::Endpoint(127,0,0,1,8000));
    
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
