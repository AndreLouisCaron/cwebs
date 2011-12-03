/*!
 * @file demo/nix/tunnel/client-tunnel.cpp
 * @brief Simple program that pipes standard streams to a websocket connection.
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include <stdlib.h>
#include <iostream>

#include "nix/Endpoint.hpp"
#include "nix/File.hpp"
#include "nix/Stream.hpp"

#include "nix/Client.hpp"

int main ( int argc, char ** argv )
try
{
    // Open both ends of the tunnel.
    nix::File host(STDIN_FILENO);
    nix::net::Stream peer(
        nix::net::Endpoint(127,0,0,1,8000));

    // Perform tunnelled data exchange.
    nix::Client(host, peer).exchange();
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
