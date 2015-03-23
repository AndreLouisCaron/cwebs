#ifndef _nix_net_Listener_hpp__
#define _nix_net_Listener_hpp__

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
 * @file demo/nix/Listener.hpp
 */

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
                close(myHandle);
                throw (Error(errno));
            }
            status = ::listen(myHandle, SOMAXCONN);
            if ( status < 0 ) {
                close(myHandle);
                throw (Error(errno));
            }
	}

    private:
        Listener ( const Listener& );

    public:
        ~Listener ()
        {
            close(myHandle);
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
