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
 * @file demo/win/Server.cpp
 */

#include "Server.hpp"

#include "b64.hpp"
#include "Digest.hpp"
#include "http.hpp"

#include <iostream>
#include <sstream>

namespace nix {

    Server::Server ( nix::File& host, nix::net::Stream& peer )
        : Tunnel(host, peer)
    {
        // Client *must* mask all frames.
        myIWire.masking_required = 1;
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
