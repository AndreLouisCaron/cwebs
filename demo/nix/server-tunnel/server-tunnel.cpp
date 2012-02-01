/*!
 * @file demo/nix/tunnel/server-tunnel.cpp
 * @brief Simple program that pipes a websocket connection to a program.
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include <cstdlib>
#include <iostream>

#include "options.hpp"

#include "nix/Endpoint.hpp"
#include "nix/File.hpp"
#include "nix/Listener.hpp"
#include "nix/Server.hpp"
#include "nix/Stream.hpp"

int main ( int argc, char ** argv )
try
{
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
    const nix::net::Endpoint endpoint =
        nix::net::Endpoint::resolve(name.c_str(), port);
    std::cerr
        << "Connecting to '" << endpoint << "'."
        << std::endl;

    // Open both ends of the tunnel.
    nix::File host(STDIN_FILENO);
    nix::net::Listener listener(endpoint);
    nix::net::Stream peer(listener);

    // Perform tunnelled data exchange.
    nix::Server(host, peer).exchange(name);
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
