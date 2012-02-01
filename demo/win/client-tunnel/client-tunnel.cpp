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
