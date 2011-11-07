#ifndef _nix_Error_hpp__
#define _nix_Error_hpp__

#include <exception>
#include <errno.h>
#include <string.h>

namespace nix {

    class Error :
      public std::exception
    {
        /* data. */
    private:
        int myCode;

        /* construction. */
    public:
        Error ( int code )
            : myCode(code)
        {}
 
        /* methods. */
    public:
        int code () const
        {
            return (myCode);
        }

        /* overrides. */
    public:
        virtual const char * what () const throw ()
        {
            return (::strerror(myCode));
        }
    };

}

#endif /* _nix_Error_hpp__ */
