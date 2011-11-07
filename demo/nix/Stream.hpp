#ifndef _nix_net_Stream_hpp__
#define _nix_net_Stream_hpp__

#include <string>
#include "Endpoint.hpp"
#include "Error.hpp"
#include "Listener.hpp"

namespace nix { namespace net {

    class Stream
    {
        /* data. */
    private:
        int myHandle;

        /* construction. */
    public:
        Stream ( Endpoint endpoint )
            : myHandle(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
        {
            if ( myHandle < 0 ) {
                throw (Error(errno));
            }
            ::sockaddr *const address =
                 reinterpret_cast< ::sockaddr* >(&endpoint.data());
            const int status = ::connect(
                myHandle, address, sizeof(endpoint.data()));
            if ( status < 0 ) {
                ::close(myHandle);
                throw (Error(errno));
            }
        }

        Stream ( Listener& listener )
            : myHandle(::accept(listener.handle(), 0, 0))
        {
            if ( myHandle < 0 ) {
                throw (Error(errno));
            }
        }

    private:
        Stream ( const Stream& );

    public:
        ~Stream ()
        {
            ::close(myHandle);
        }

        /* methods. */
    public:
        int handle () const
        {
            return (myHandle);
        }

        ssize_t get ( void * data, size_t size )
        {
            const ssize_t status = ::recv(myHandle, data, size, 0);
            if ( status < 0 ) {
                throw (Error(errno));
            }
            return (status);
        }

        void getall ( void * data, size_t size )
        {
            getall(static_cast<char*>(data), size);
        }

        void getall ( char * data, size_t size )
        {
            ssize_t used = 0;
            ssize_t pass = 0;
            do {
                pass = get(data+used, size-used);
            }
            while ((pass > 0) && ((used+=pass) < size));
        }

        ssize_t put ( const void * data, size_t size )
        {
            const ssize_t status = ::send(myHandle, data, size, 0);
            if ( status < 0 ) {
                throw (Error(errno));
            }
            return (status);
        }

        void putall ( const void * data, size_t size )
        {
            putall(static_cast<const char*>(data), size);
        }

        void putall ( const std::string& message )
        {
            putall(message.data(), message.size());
        }

        void putall ( const char * data, size_t size )
        {
            ssize_t used = 0;
            ssize_t pass = 0;
            do {
                pass = put(data+used, size-used);
            }
            while ((pass > 0) && ((used+=pass) < size));
        }

        void shutdowni ()
        {
            const int status = ::shutdown(myHandle, SHUT_RD);
            if ( status < 0 ) {
                throw (Error(errno));
            }
        }

        void shutdowno ()
        {
            const int status = ::shutdown(myHandle, SHUT_WR);
            if ( status < 0 ) {
                throw (Error(errno));
            }
        }

        void shutdown ()
        {
            const int status = ::shutdown(myHandle, SHUT_RDWR);
            if ( status < 0 ) {
                throw (Error(errno));
            }
        }

      /* operators. */
    private:
        Stream& operator= ( const Stream& );
    };

} }

#endif /* _nix_net_Stream_hpp__ */
