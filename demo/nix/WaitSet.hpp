#ifndef _nix_WaitSet_hpp__
#define _nix_WaitSet_hpp__

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
 * @file nix/WaitSet.hpp
 */

#include <sys/select.h>
#include <cstring>
#include <iostream>

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
            return (mySize+1);
        }

        void clear ()
        {
            FD_ZERO(&myData);
        }

        bool contains ( int handle ) const
        {
            return (FD_ISSET(handle, &myData));
        }

        WaitSet& add ( int handle )
        {
            FD_SET(handle, &myData);
            mySize = MAX(mySize, handle);
            return (*this);
        }

        WaitSet& del ( int handle )
        {
            FD_CLR(handle, &myData); return (*this);
        }
    };

    inline int waitfori ( WaitSet& pull )
    {
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
