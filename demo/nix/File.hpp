#ifndef _nix_File_hpp__
#define _nix_File_hpp__

#include <sys/fcntl.h>
#include <string>
#include "Error.hpp"

namespace nix {

    class File
    {
        /* data. */
    private:
        int myHandle;

        /* construction. */
    public:
        File ( const char * path )
            : myHandle(::open(path, O_RDONLY))
        {
            if ( myHandle < 0 ) {
                throw (Error(errno));
            }
        }

        explicit File ( int handle )
            : myHandle(handle)
        {
        }

    private:
        File ( const File& );

    public:
        ~File ()
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
            const ssize_t status = ::read(myHandle, data, size);
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
            const ssize_t status = ::write(myHandle, data, size);
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

        /* operators. */
    private:
        File& operator= ( const File& );
    };

}

#endif /* _nix_File_hpp__ */
