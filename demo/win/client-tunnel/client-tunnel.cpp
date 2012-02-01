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

/*!
 * @file demo/win/client-tunnel/client-tunnel.cpp
 * @brief Simple program that pipes standard streams to a websocket connection.
 */

#include <cstdlib>
#include <iostream>

#include "options.hpp"
#include "win/Context.hpp"
#include "win/Client.hpp"
#include "win/Error.hpp"
#include "win/Endpoint.hpp"
#include "win/Stdin.hpp"
#include "win/Stream.hpp"

int main ( int argc, char ** argv )
try
{
    using win::net::uint16_t;

    const win::net::Context context;

    // Get the host name.
    if (argc < 2)
    {
        std::cerr
            << "Host name or IP address required."
            << std::endl;
        return (EXIT_FAILURE);
    }
    const std::string name(argv[1]);

    // Get the port number.
    const uint16_t port = ::getarg<uint16_t>(argc-1, argv+1, "-p", 80);

    // Assemble the IP end point.
    const win::net::Endpoint endpoint =
        win::net::Endpoint::resolve(name.c_str(), port);
    std::cerr
        << "Conneting to '" << endpoint << "'."
        << std::endl;

    // Open both ends of the tunnel.
    win::Stdin host;
    win::net::Stream peer(endpoint);

    // Perform tunnelled data exchange.
    win::Client(host, peer).exchange(name);
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
