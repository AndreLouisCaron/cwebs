#ifndef _nix_File_hpp__
#define _nix_File_hpp__

// Copyright (c) 2011-2012, Andre Caron (andre.l.caron@gmail.com)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// Copyright(c) 2011, Andre Caron (andre.l.caron@gmail.com)

/*!
 * @file demo/nix/File.hpp
 */

#include <sys/fcntl.h>
#include <string>
#include <unistd.h>
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
            close(myHandle);
        }

        /* methods. */
    public:
        int handle () const
        {
            return (myHandle);
        }

        ssize_t get ( void * data, size_t size )
        {
            const ssize_t status = read(myHandle, data, size);
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
