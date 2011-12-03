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

#include "win/Context.hpp"
#include "win/Error.hpp"
#include "win/Endpoint.hpp"
#include "win/Stdin.hpp"
#include "win/Stream.hpp"

#include "win/Client.hpp"

int main ( int argc, char ** argv )
try
{
    // Open both ends of the tunnel.
    win::Stdin host;
    const win::net::Context context;
    win::net::Stream peer(
        win::net::Endpoint(192,168,200,129,8000));

    // Perform tunnelled data exchange.
    win::Client(host, peer).exchange();
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
