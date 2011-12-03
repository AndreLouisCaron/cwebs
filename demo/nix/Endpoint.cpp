/*!
 * @file nix/Endpoint.cpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 * @brief IPv4 endpoint.
 */

#include "Endpoint.hpp"
#include "Error.hpp"
#include <netdb.h>
#include <string.h>

namespace nix { namespace net {

    const Endpoint Endpoint::resolve ( const char * name )
    {
        ::addrinfo hint; ::memset(&hint, 0, sizeof(hint));
        hint.ai_family   = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = IPPROTO_TCP;
        ::addrinfo * peer = 0;
        const int result = ::getaddrinfo(name, "http", &hint, &peer);
        if (result != 0) {
            throw Error(result);
        }
        const Endpoint endpoint(
            *reinterpret_cast<const ::sockaddr_in*>(peer->ai_addr));
        ::freeaddrinfo(peer);
        return (endpoint);
    }

    const Endpoint Endpoint::any ( uint16_t port )
    {
        ::sockaddr_in data;
        ::memset(&data, 0, sizeof(data));
        data.sin_family = AF_INET;
        data.sin_addr.s_addr = INADDR_ANY;
        data.sin_port = ::htons(port);
	return (Endpoint(data));
    }

    const Endpoint Endpoint::localhost ( uint16_t port )
    {
      return (Endpoint(127,0,0,1,port));
    }

    Endpoint::Endpoint ( const ::sockaddr_in& data )
    {
      ::memcpy(&myData, &data, sizeof(myData));
    }

} }
