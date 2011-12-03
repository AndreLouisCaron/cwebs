// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// This document is covered by the an Open Source Initiative (OSI) approved
// license.  A copy of the license should have been provided alongside this
// software package (see "license.rtf"). If not, the license is available online
// at "http://www.opensource.org/licenses/BSD-3-Clause".

/*!
 * @file demo/win/server-tunnel/server-tunnel.cpp
 * @brief Simple program that pipes a websocket connection to a program.
 *
 * @author Andre Caron (andre.l.caron@gmail.com)
 */

#include <stdlib.h>
#include <iostream>

#include "win/Context.hpp"
#include "win/Error.hpp"
#include "win/Endpoint.hpp"
#include "win/Listener.hpp"
#include "win/Stdin.hpp"
#include "win/Stream.hpp"

#include "win/Server.hpp"

int main ( int argc, char ** argv )
try
{
    // Open both ends of the tunnel.
    win::Stdin host;
    const win::net::Context context;
    win::net::Listener listener(
        win::net::Endpoint(127,0,0,1,8000));
    win::net::Stream peer(listener);

    // Perform tunnelled data exchange.
    win::Server(host, peer).exchange();
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
