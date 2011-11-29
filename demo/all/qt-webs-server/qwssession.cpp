// Copyright(c) Andre Caron <andre.l.caron@gmail.com>, 2011
//
// This document is covered by the an Open Source Initiative approved license. A
// copy of the license should have been provided alongside this software package
// (see "LICENSE.txt"). If not, terms of the license are available online at
// "http://www.opensource.org/licenses/mit".

#include "qwssession.hpp"

#include <algorithm>
#include <sstream>

#include <QDebug>

namespace {

    class Lower {
        std::locale myLocale;
    public:
        Lower ( const std::locale& locale=std::locale() )
            : myLocale(locale)
        {}
        char operator() ( char x ) const {
            return (std::tolower(x, myLocale));
        }
    };

    const std::string lower ( std::string x ) {
        std::transform(x.begin(), x.end(), x.begin(), Lower()); return (x);
    }

    // Refer to websocket specification for details.
    const std::string accept_key ( const std::string& skey )
    {
        static const std::string guid
            ("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(skey.data(), skey.size());
        hash.addData(guid.data(), guid.size());
        const QByteArray result = hash.result().toBase64();
        return (std::string(result.data(), result.size()));
    }

    void print ( const std::string& message )
    {
        qDebug() << message.c_str();
    }

}

namespace qws {

    Session::Session ( QTcpSocket * socket, QObject * parent )
        : QObject(parent), mySocket(socket), myState(Connecting)
    {
          // Configure in-bound web socket traffic.
        ::ws_iwire_init(&myIWire);
        myIWire.baton = this;
        myIWire.new_message    = &Session::new_message;
        myIWire.end_message    = &Session::end_message;
        myIWire.new_fragment   = &Session::new_frame;
        myIWire.end_fragment   = &Session::end_frame;
        myIWire.accept_content = &Session::accept_ibound_content;
        
          // Configure out-bound web socket traffic.
        ::ws_owire_init(&myOWire);
        myOWire.baton = this;
        myOWire.accept_content = &Session::accept_obound_content;
        
          // Delete this wrapper object when the socket is disconnected.
        QObject::connect(
            socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
        
          // Get notified when data arrives on the socket.  Make sure this call
          // is last.  Don't start processing data until we finished configuring
          // the object.
        QObject::connect(
            socket, SIGNAL(readyRead()), this, SLOT(consume()));
    }

    Session::~Session ()
    {
    }

    void Session::autopong ()
    {
        QObject::connect(
            this, SIGNAL(ping(const void*,quint64)),
            this,   SLOT(pong(const void*,quint64)));
    }

    void Session::sendtext ( const void * data, quint64 size )
    {
        ::ws_owire_put_text(&myOWire, data, size);
    }

    void Session::sendtext ( const QString& payload )
    {
        sendtext(payload.toUtf8());
    }

    void Session::sendtext ( const QByteArray& payload )
    {
        sendtext(payload.data(), payload.size());
    }

    void Session::senddata ( const void * data, quint64 size )
    {
        ::ws_owire_put_data(&myOWire, data, size);
    }

    void Session::senddata ( const QByteArray& payload )
    {
        senddata(payload.data(), payload.size());
    }

    void Session::ping ()
    {
        ::ws_owire_put_ping(&myOWire, 0, 0);
    }

    void Session::ping ( const void * data, quint64 size )
    {
        ::ws_owire_put_ping(&myOWire, data, size);
    }

    void Session::pong ()
    {
        ::ws_owire_put_pong(&myOWire, 0, 0);
    }

    void Session::pong ( const void * data, quint64 size )
    {
        ::ws_owire_put_pong(&myOWire, data, size);
    }

    void Session::close ()
    {
        ::ws_owire_put_kill(&myOWire, 0, 0);
    }

    void Session::close ( const void * data, quint64 size )
    {
        ::ws_owire_put_kill(&myOWire, data, size);
    }

    void Session::consume ()
    {
        if ( myState == Connecting )
        {
              // Read all available data and feed websocket parser.
            char data[1024];
            qint64 size = 0;
            do {
                size = mySocket->read(data, sizeof(data));
                ::ws_iwire_feed(&myIWire, data, size);
            }
            while ( size > 0 );
        }
          // Complete web socket handshake.
        else if ( myState == Open )
        {
              // Read all available data and feed HTTP parser.
            char data[1024];
            qint64 size = 0;
            qint64 used = 0;
            do {
                size = mySocket->read(data, sizeof(data));
                used = myRequest.feed(data, size);
            }
            while ((size > 0) && (used == size) && !myRequest.complete());
            
              // Repeat until request is complete.
            if ( !myRequest.complete() ) {
                return;
            }
            emit request();
            
              // Make sure this is a web socket upgrade request.
            // Check 'Host' header.
            // Check 'Origin' header.
            // Check 'Connection' header.
            if ( !myRequest.upgrade() ||
                (myRequest.header("Upgrade") != "websocket"))
            {
                print("Not an upgrade request!");
                emit invalid();
                  // Fail the web socket connection.
                shutdown();
                return;
            }
            
              // Check client/server compatibility.
            // Check 'Sec-WebSocket-Version' header.
            // Check 'Sec-WebSocket-Protocol' header.
            // Check 'Sec-WebSocket-Extensions' header.
            // ...
            
              // Check 'Sec-WebSocket-Key' header.
            const std::string key =
                ::accept_key(myRequest.header("Sec-WebSocket-Key"));
            
              // Confirm handshake completion.
            std::string response;
            {
                std::ostringstream stream;
                stream
                    << "HTTP/1.1 101 Switching Protocols\r\n"
                    << "Upgrade: websocket\r\n"
                    << "Connection: Upgrade\r\n"
                    << "Sec-WebSocket-Accept: " << key << "\r\n"
                    << "Sec-WebSocket-Version: 13" << "\r\n"
                    << "\r\n";
                response = stream.str();
            }
            mySocket->write(response.data(), response.size());
            
              // Connection upgrade successful.
            myState = Open;
            emit upgrade();
            
              // Forward any data following HTTP request.
            if ( used < size ) {
                ::ws_iwire_feed(&myIWire, data+used, size-used);
            }
            
              // Check right away for available data.
            emit consume();
        }
    }

    void Session::shutdown ()
    {
          // Start websocket closing handshake.
        ::ws_owire_put_kill(&myOWire, 0, 0);
        myState = Closing;
          // Initiate TCP shutdown.
        mySocket->disconnectFromHost();
    }

    void Session::kill ()
    {
          // Close socket without closing handshake or TCP shutdown.
        mySocket->abort();
    }

    void Session::new_message ( ::ws_iwire * backend )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        emit client.newmessage();
    }

    void Session::end_message ( ::ws_iwire * backend )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        emit client.endmessage();
        if ( ::ws_iwire_dead(backend) ) {
            emit client.closed();
        }
    }

    void Session::new_frame ( ::ws_iwire * backend, uint64 size )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        emit client.newframe(size);
    }

    void Session::end_frame ( ::ws_iwire * backend )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        emit client.endframe();
    }

    void Session::accept_ibound_content (
        ::ws_iwire * backend, const void * data, uint64 size )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        if ( ::ws_iwire_ping(backend) ) {
            emit client.pinged(data, size);
        }
        if ( ::ws_iwire_pong(backend) ) {
            emit client.ponged(data, size);
        }
        if ( ::ws_iwire_text(backend) ) {
            emit client.text(data, size);
        }
        if ( ::ws_iwire_data(backend) ) {
            emit client.data(data, size);
        }
    }

    void Session::accept_obound_content (
        ::ws_owire * backend, const void * data, uint64 size )
    {
        Session& client = *static_cast<Session*>(backend->baton);
        quint64 used = 0;
        do {
            used += client.mySocket->write(
                static_cast<const char*>(data)+used, size-used);
        }
        while ( used < size );
    }

}
