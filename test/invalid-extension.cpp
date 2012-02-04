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

/*!
 * @file test/invalid-extension.cpp
 * @brief Tests error reporting for frames containing invalid extension codes.
 */

#include "unit-test.hpp"

namespace {

    int test ( int argc, char ** argv )
    {
        // try out all possible combinations of extension codes and masks.
        for (uint8 mask=0; mask < 8; ++mask)
        {
            for (uint8 code=0; code < 8; ++code)
            {
                // get a fresh parser.
                ::ws_iwire wire;
                ::ws_iwire_init(&wire);
                wire.extension_mask = mask;

                // feed the first byte of a message header.
                uint8 head = 0x80|(code<<4)|0x01;
                ::ws_iwire_feed(&wire, &head, 1);

                // check that an error is triggered when we expect one.
                bool expect = ((code & ~mask) != 0);
                bool result = (wire.status == ws_iwire_invalid_extension);
                if (expect != result)
                {
                    // show which combination failed.
                    std::cerr
                        << "mask: " << int(mask) << std::endl
                        << "code: " << int(code) << std::endl
                        << "expect: " << expect << std::endl
                        << "result: " << result << std::endl
                        ;
                    return (FAIL);
                }
            }
        }

        return (PASS);
    }

}

#include "unit-test.cpp"
