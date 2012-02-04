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

#include "unit-test.hpp"

namespace {

    class Stream
    {
        /* data. */
    private:
        std::ostream& myBackend;
        ::ws_iwire myWire;
        sha1::Digest myDigest;
        std::size_t myMessages;
        std::size_t myFragments;

        /* construction. */
    public:
        Stream ( std::ostream& backend )
            : myBackend(backend)
        {
            ::ws_iwire_init(&myWire);
            myWire.baton = this;
            myWire.new_message    = &Stream::new_message;
            myWire.end_message    = &Stream::end_message;
            myWire.new_fragment   = &Stream::new_fragment;
            myWire.end_fragment   = &Stream::end_fragment;
            myWire.accept_content = &Stream::accept_content;

            myMessages = 0;
            myBackend << '[' << std::endl;
        }

        ~Stream ()
        {
            myBackend << ']' << std::endl;
        }

        /* methods. */
    public:
        bool feed ( const void * data, uint64 size )
        {
            return (::ws_iwire_feed(&myWire, data, size) == size);
        }

    private:
        void new_message ( ::ws_iwire& wire )
        {
            myFragments = 0;
            myBackend << ' ' << myMessages << "={";
        }

        void end_message ( ::ws_iwire& wire )
        {
            myBackend << '}' << std::endl;
        }

        void new_fragment ( ::ws_iwire& wire, uint64 size )
        {
            myDigest.clear();
            if (myFragments++ > 0) {
                myBackend << "    ";
            }
            myBackend << '(';
            if (::ws_iwire_text(&wire)) {
                myBackend << "text";
            }
            if (::ws_iwire_data(&wire)) {
                myBackend << "data";
            }
            if (::ws_iwire_ping(&wire)) {
                myBackend << "ping";
            }
            if (::ws_iwire_pong(&wire)) {
                myBackend << "pong";
            }
            myBackend
                << ',' << (::ws_iwire_mask(&wire)?'m':'p')
                << ',' << size
                ;
        }

        void end_fragment ( ::ws_iwire& wire )
        {
            myBackend
                << ',' << (std::string)myDigest.result()
                << ')';
            if (!::ws_iwire_last_fragment(&wire)) {
                myBackend << std::endl;
            }
        }

        void accept_content ( ::ws_iwire& wire, const void * data, uint64 size )
        {
            const uint64 ubnd =
                static_cast<uint64>(std::numeric_limits<std::size_t>::max());
            for (uint64 used = 0; used < size;)
            {
                const std::size_t pass =
                    static_cast<std::size_t>(std::min(ubnd, size-used));
                myDigest.update(data, pass), used += pass;
            }
        }

        /* class methods. */
    private:
        static void new_message ( ws_iwire * wire )
        {
            static_cast<Stream*>(wire->baton)->new_message(*wire);
        }

        static void end_message ( ws_iwire * wire )
        {
            static_cast<Stream*>(wire->baton)->end_message(*wire);
        }

        static void new_fragment ( ws_iwire* wire, uint64 size )
        {
            static_cast<Stream*>(wire->baton)->new_fragment(*wire, size);
        }

        static void end_fragment ( ws_iwire* wire )
        {
            static_cast<Stream*>(wire->baton)->end_fragment(*wire);
        }

        static void accept_content
            ( ::ws_iwire * wire, const void * data, uint64 size )
        {
            static_cast<Stream*>(wire->baton)
                ->accept_content(*wire, data, size);
        }
    };

    int test ( int argc, char ** argv )
    {
        for (int i=0; i < argc; ++i)
        {
            std::ifstream file(argv[i]);
            if (!file.is_open())
            {
                std::cerr
                    << "Could not open input file '" << argv[i] << "'."
                    << std::endl;
                return (FAIL);
            }
            Stream stream(std::cout);
            for (char data[1024]; file.read(data, sizeof(data)).gcount() > 0;)
            {
                if (!stream.feed(data, file.gcount()))
                {
                    std::cerr
                        << "Error parsing records."
                        << std::endl;
                    return (FAIL);
                }
            }
            if (!file.eof())
            {
                std::cerr
                    << "Error reading from input file '" << argv[i]
                    << "' at position " << file.tellg() << "."
                    << std::endl;
                return (FAIL);
            }
        }
        return (PASS);
    }

}

#include "unit-test.cpp"
