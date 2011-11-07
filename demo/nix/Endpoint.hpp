#ifndef _nix_Endpoint_hpp__
#define _nix_Endpoint_hpp__

/*!
 * @file nix/Endpoint.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 * @brief IPv4 endpoint.
 */

#include <stdint.h>
#include <netinet/in.h>

namespace nix { namespace net {

    class Endpoint
    {
        /* data. */
    private:
        ::sockaddr_in myData;

        /* construction. */
    public:
        Endpoint ( uint16_t port )
        {
            myData.sin_family = AF_INET;
            myData.sin_addr.s_addr = INADDR_ANY;
            myData.sin_port = ::htons(port);
        }

        Endpoint ( uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port )
        {
            myData.sin_family = AF_INET;
            myData.sin_addr.s_addr =
                (uint32_t(d) << 24)|
                (uint32_t(c) << 16)|
                (uint32_t(b) <<  8)|
                (uint32_t(a) <<  0);
            myData.sin_port = ::htons(port);
        }

        /* methods. */
    public:
        ::sockaddr_in& data ()
        {
            return (myData);
        }

        const ::sockaddr_in& data () const
        {
            return (myData);
        }
    };

} }

#endif /* _nix_Endpoint_hpp__ */
