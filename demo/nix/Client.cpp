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
 * @file demo/nix/Client.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Client.hpp"

#include "b64.hpp"
#include "Digest.hpp"
#include "http.hpp"

#include <iostream>
#include <sstream>

namespace nix {

    Client::Client ( nix::File& host, nix::net::Stream& peer )
        : Tunnel(host, peer)
    {
        // Client *must* mask all frames.
        myOWire.auto_mask = 1;
    }

    void Client::handshake ( const std::string& host )
    {
        char data[1024];
        ::ws_iwire_feed(&myIWire, data,
            handshake(host, data, sizeof(data)));
    }

    std::size_t Client::handshake
        ( const std::string& host, char * data, std::size_t size )
    {
        // Generate a nonce.
        std::string nonce(16, '\0');
        { 
            nix::File stream("/dev/random");
            char random[16];
            stream.get(random, 16);
            nonce.assign(random, 16);
        }
        nonce = b64::encode(nonce);
        
        // Send HTTP upgrade request.
        std::ostringstream request;
        request
            << "GET / HTTP/1.1"               << "\r\n"
            << "Host: " << host               << "\r\n"
            << "Upgrade: websocket"           << "\r\n"
            << "Connection: upgrade"          << "\r\n"
            << "Sec-WebSocket-Key: " << nonce << "\r\n"
            << "Sec-WebSocket-Version: 13"    << "\r\n"
            << "\r\n";
        myPeer.putall(request.str());
        
        // Wait for upgrade approval.
        http::Response response;
        ::size_t used = 0;
        ::size_t pass = 0;
        do {
            pass = myPeer.get(data, size);
            if ( pass == 0 ) {
                std::cerr << "Peer has finished." << std::endl;
                break;
            }
            used = response.feed(data, pass);
        }
        while ( !response.complete() );

        // Move leftover data at the beginning of the buffer.
        std::copy(data+used, data+pass, data);
        
        // Make sure we succeeded.
        if (!response.complete()) {
            std::cerr << "Did not finish HTTP response." << std::endl;
        }
        
        // Confirm handshake.
        if (!http::ieq(response.header("Connection"),"Upgrade"  )||
            !http::ieq(response.header("Upgrade"   ),"WebSocket"))
        {
            std::cerr << "Upgrade request denied." << std::endl;
        }
        const std::string version = response.header("Sec-WebSocket-Version");
        const std::string key = response.header("Sec-WebSocket-Accept");
        if (key != approve_nonce(nonce)) {
            std::cerr << "Invalid nonce reply." << std::endl;
        }

        // Keep any leftovers for the wire protocol.
        return (pass-used);
    }

}
