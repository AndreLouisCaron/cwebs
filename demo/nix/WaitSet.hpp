#ifndef _nix_WaitSet_hpp__
#define _nix_WaitSet_hpp__

/*!
 * @file nix/WaitSet.hpp
 * @author Andre Caron (andre.l.caron@gmail.com)
 * @brief 
 */

#include <sys/select.h>
#include <string.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

namespace nix {

    class WaitSet
    {
        /* data. */
    private:
        ::fd_set myData;
        int mySize;

        /* construction. */
    public:
        WaitSet ()
            : mySize(0)
        {
            clear();
        }

        WaitSet ( const WaitSet& waitables )
        {
            ::memcpy(&myData, &waitables.myData, sizeof(::fd_set));
            mySize = waitables.mySize;
        }

        ~WaitSet ()
        {
            clear();
        }

        /* methods. */
    public:
        int capacity () const
        {
            return (FD_SETSIZE);
        }

        ::fd_set& data ()
        {
            return (myData);
        }

        const ::fd_set& data () const
        {
            return (myData);
        }

        int size () const
        {
	  std::cout << "(nfds): " << (mySize+1) << std::endl;
            return (mySize+1);
        }

        void clear ()
        {
	  std::cout << "(clear)" << std::endl;
            FD_ZERO(&myData);
        }

        bool contains ( int handle ) const
        {
	  std::cout << "(contains? fd=" << handle << "): "
		    << FD_ISSET(handle,&myData) << std::endl;
            return (FD_ISSET(handle, &myData));
        }

        WaitSet& add ( int handle )
        {
	  std::cout << "(add fd=" << handle << ")" << std::endl;
            FD_SET(handle, &myData);
            mySize = MAX(mySize, handle);
            return (*this);
        }

        WaitSet& del ( int handle )
        {
	  std::cout << "(del fd=" << handle << ")" << std::endl;
            FD_CLR(handle, &myData); return (*this);
        }
    };

    inline int waitfori ( WaitSet& pull )
    {
        std::cout
	    << "NFDS: '" << pull.size() << "'."
	    << std::endl;
        const int status = ::select(pull.size(), &pull.data(), 0, 0, 0);
        if ( status == -1 ) {
            std::cerr << "Select!" << std::endl;
        }
        return (status);
    }

    inline void waitforo ( WaitSet& push )
    {
        const int status = ::select(push.size(), 0, &push.data(), 0, 0);
        if ( status == -1 ) {
            std::cerr << "Select!" << std::endl;
        }
    }

    inline bool waitfore ( WaitSet& fail )
    {
        const int status = ::select(fail.size(), 0, 0, &fail.data(), 0);
        if ( status == -1 ) {
            std::cerr << "Select!" << std::endl;
        }
    }

}

#endif /* _nix_WaitSet_hpp__ */
