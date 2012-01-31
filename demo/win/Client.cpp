// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/Client.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include "Client.hpp"

#include "b64.hpp"
#include "Digest.hpp"
#include "http.hpp"

#include <sstream>

namespace win {

    Client::Client ( win::Stdin& host, win::net::Stream& peer )
        : Tunnel(host, peer)
    {
        // Client *must* mask all frames.
        myOWire.auto_mask = 1;
    }

    void Client::handshake ()
    {
        char data[1024];
        ::ws_iwire_feed(&myIWire, data,
            handshake(myPeer, data, sizeof(data)));
    }

    std::size_t Client::handshake
        ( win::net::Stream& peer, char * data, std::size_t size )
    {
        // Generate a nonce.
        std::string nonce(16, '\0');
        { 
            char random[16];
            generate_nonce(random, 16);
            nonce.assign(random, 16);
        }
        nonce = b64::encode(nonce);
        
        // Send HTTP upgrade request.
        std::ostringstream request;
        request
            << "GET / HTTP/1.1"               << "\r\n"
            << "Host: ..."                    << "\r\n"
            << "Upgrade: WebSocket"           << "\r\n"
            << "Connection: Upgrade"          << "\r\n"
            << "Sec-WebSocket-Key: " << nonce << "\r\n"
            << "Sec-WebSocket-Version: 13"    << "\r\n"
            << "\r\n";
        peer.putall(request.str());
        
        // Wait for upgrade approval.
        http::Response response;
        ::size_t used = 0;
        ::size_t pass = 0;
        do {
            pass = peer.get(data, size);
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
