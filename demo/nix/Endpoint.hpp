#ifndef _nix_Endpoint_hpp__
#define _nix_Endpoint_hpp__

/*!
 * @file nix/Endpoint.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 * @brief IPv4 endpoint.
 */

#include <stdint.h>
#include <netinet/in.h>
#include <iosfwd>

namespace nix { namespace net {

    class Endpoint
    {
        /* data. */
    private:
        ::sockaddr_in myData;

        /* class methods. */
    public:
        static const Endpoint resolve ( const char * name );
        static const Endpoint resolve ( const char * name, uint16_t port );
        static const Endpoint resolve ( const char * name, const char * port );

        static const Endpoint any ( uint16_t port );
        static const Endpoint localhost ( uint16_t port );

        /* construction. */
    public:
        Endpoint ( const ::sockaddr_in& data );
        Endpoint ( const ::sockaddr_in& data, uint16_t port );

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

    std::ostream& operator<<
        ( std::ostream& stream, const Endpoint& endpoint );

} }

#endif /* _nix_Endpoint_hpp__ */
