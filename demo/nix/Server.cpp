// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Server.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Server.hpp"

#include "b64.hpp"
#include "Digest.hpp"
#include "http.hpp"

#include <sstream>

namespace nix {

    Server::Server ( nix::File& host, nix::net::Stream& peer )
        : Tunnel(host, peer)
    {
    }

    void Server::handshake ( const std::string& host )
    {
        char data[1024];
        ::ws_iwire_feed(&myIWire, data,
            handshake(host, data, sizeof(data)));
    }

    std::size_t Server::handshake
        ( const std::string& host, char * data, std::size_t size )
    {
        http::Request request;
        ::size_t used = 0;
        do {
            used = myPeer.get(data, size);
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
        if (!http::ieq(request.header("Connection"),"Upgrade"  )||
            !http::ieq(request.header("Upgrade"   ),"WebSocket"))
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
        std::string key = Tunnel::approve_nonce(nonce);
        
        // Send HTTP upgrade approval.
        std::ostringstream response;
        response
            << "HTTP/1.1 101 Switching Protocols" << "\r\n"
            << "Upgrade: websocket"               << "\r\n"
            << "Connection: upgrade"              << "\r\n"
            << "Sec-WebSocket-Accept: " << key    << "\r\n"
            << "Sec-WebSocket-Version: 13"        << "\r\n"
            << "\r\n";
        myPeer.putall(response.str());
        
        // Keep any leftovers for the wire protocol.
        return (used);
    }

}
