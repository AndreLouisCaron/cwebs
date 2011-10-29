#ifndef _ws_hpp__
#define _ws_hpp__

/*!
 * @file ws.hpp
 * @brief Web Socket streaming parser for C++.
 * @see http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17
 *
 * @author Andre Caron (andre.louis.caron@usherbrooke.ca)
 */

#include "ws.h"
#include <vector>
#include <iostream>
#include <string>

/*!
 * @brief Web Socket implementation.
 */
namespace webs {

    /*!
     * @brief Streaming parser for Web Socket wire protocol.
     */
    class iwire
    {
        /* data. */
    private:
        std::ostream& myOutput;
        ::ws_iwire myBackend;

        /* construction. */
    public:
        /*!
         * @brief
         */
        iwire ( std::ostream& output=std::cout )
            : myOutput(output)
        {
            ::ws_iwire_init(&myBackend);
            myBackend.baton = this;
            myBackend.new_message = &iwire::new_message;
            myBackend.end_message = &iwire::end_message;
            myBackend.new_fragment = &iwire::new_fragment;
            myBackend.end_fragment = &iwire::end_fragment;
            myBackend.accept_content = &iwire::accept_content;
        }

        /* methods. */
    public:
        uint64 feed ( const void * data, uint64 size )
        {
            return (::ws_iwire_feed(&myBackend, data, size));
        }

        bool dead () const
        {
            return (::ws_iwire_dead(&myBackend) == 0);
        }

    private:
        void new_message ()
        {
            myOutput
                << "Message!" << std::endl;
        }

        void end_message ()
        {
            myOutput
                << std::endl;
        }

        void new_fragment ( uint64 size )
        {
            myOutput << "Fragment (" << size << "): '";
        }

        void end_fragment ()
        {
            myOutput
                << "'." << std::endl;
        }

          // data pushed over communications channel.
        void accept_content ( const void * data, uint64 size )
        {
            if ( ::ws_iwire_ping(&myBackend) )
            {
                  // send pong.
                // ...
                // skip body.
                myOutput
                    << "Ping!";
            }
            if ( ::ws_iwire_pong(&myBackend) )
            {
                // skip body.
                myOutput
                    << "Pong!";
            }
            if ( ::ws_iwire_text(&myBackend) )
            {
                  // output text.
                for ( uint64 used = 0; (used < size); )
                {
                    const std::streamsize n =
                        static_cast<std::streamsize>(size-used);
                    myOutput.write(static_cast<const char*>(data)+used, n);
                    used += n;
                }
            }
            if ( ::ws_iwire_data(&myBackend) )
            {
                  // output data as hex.
                myOutput << std::hex;
                for ( uint64 used = 0; (used < size); ++used ) {
                    myOutput
                        << (int)static_cast<const unsigned char*>(data)[used];
                }
                myOutput << std::dec;
            }
        }

        /* class methods. */
    private:
        static void new_message ( ::ws_iwire * backend )
        {
            static_cast<iwire*>(backend->baton)->new_message();
        }

        static void end_message ( ::ws_iwire * backend )
        {
            static_cast<iwire*>(backend->baton)->end_message();
        }

        static void new_fragment ( ::ws_iwire * backend, uint64 size )
        {
            static_cast<iwire*>(backend->baton)->new_fragment(size);
        }

        static void end_fragment ( ::ws_iwire * backend )
        {
            static_cast<iwire*>(backend->baton)->end_fragment();
        }

        static void accept_content (
            ::ws_iwire * backend, const void * data, uint64 size )
        {
            static_cast<iwire*>(backend->baton)->accept_content(data, size);
        }
    };

    class owire
    {
        /* data. */
    private:
        std::ostream& myOutput;
        iwire myParser;
        ::ws_owire myBackend;

        /* construction. */
    public:
        owire ( std::ostream& output=std::cout )
            : myOutput(output), myParser(myOutput)
        {
            ::ws_owire_init(&myBackend);
            myBackend.baton = this;
            myBackend.accept_content = &owire::accept_content;
            myBackend.auto_fragment = 4;
            myBackend.auto_mask = 1;
        }

        /* methods. */
    public:
        void put_text ( const std::string& text )
        {
            ::ws_owire_put_text(&myBackend, text.data(), text.size());
        }

        void put_ping ()
        {
            ::ws_owire_put_ping(&myBackend, 0, 0);
        }

        void put_pong ()
        {
            ::ws_owire_put_pong(&myBackend, 0, 0);
        }

    private:
          // data to push over communications channel.
        void accept_content ( const void * data, uint64 size )
        {
            myParser.feed(data, size);
        }

        /* class methods. */
    private:
        static void accept_content
            ( ::ws_owire * backend, const void * data, uint64 size )
        {
            static_cast<owire*>(backend->baton)->accept_content(data, size);
        }
    };

}

#endif /* _ws_hpp__ */

