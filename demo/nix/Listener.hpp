#ifndef _nix_net_Listener_hpp__
#define _nix_net_Listener_hpp__

namespace nix { namespace net {

    class Listener
    {
        /* data. */
    private:
        int myHandle;

        /* construction. */
    public:
        Listener ( Endpoint endpoint )
            : myHandle(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
        {
            if ( myHandle < 0 ) {
                throw (Error(errno));
            }
            ::sockaddr *const address =
                 reinterpret_cast< ::sockaddr* >(&endpoint.data());
            int status = ::bind(
                myHandle, address, sizeof(endpoint.data()));
            if ( status < 0 ) {
                ::close(myHandle);
                throw (Error(errno));
            }
            status = ::listen(myHandle, SOMAXCONN);
            if ( status < 0 ) {
                ::close(myHandle);
                throw (Error(errno));
            }
	}

    private:
        Listener ( const Listener& );

    public:
        ~Listener ()
        {
            ::close(myHandle);
        }

        /* methods. */
    public:
        int handle () const
        {
            return (myHandle);
        }
    };

} }

#endif /* _nix_net_Listener_hpp__ */
