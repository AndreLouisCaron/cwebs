#ifndef SESSION_HPP
#define SESSION_HPP

// Copyright(c) Andre Caron <andre.louis.caron@usherbrooke.ca>, 2011
//
// This document is covered by the an Open Source Initiative approved license. A
// copy of the license should have been provided alongside this software package
// (see "LICENSE.txt"). If not, terms of the license are available online at
// "http://www.opensource.org/licenses/mit".

#include "ws.hpp"
#include "Request.hpp"

#include <QtNetwork>
#include <QObject>
#include <QScopedPointer>

namespace qws {

    /*!
     * @brief Server-side handler for a websocket connection.
     *
     * This class is mostly a high-level interface to a websocket connection.
     *
     * This class is re-entrant (multi-threaded use is safe if each instance is
     * used by only one thread).  Usualy QObject threading constraints apply.
     */
    class Session :
        public QObject
    {
        Q_OBJECT;

        /* nested types. */
    private:
        enum State
        {
            Connecting,
            Open,
            Closing, // initiated by host or peer?
            Closed,
        };

        /* data. */
    private:
        const QScopedPointer<QTcpSocket> mySocket;
        State myState;

          // Low-level protocol handlers.
        http::Request myRequest;
        ::ws_iwire myIWire;
        ::ws_owire myOWire;

        /* construction. */
    public:
        /*!
         * @brief Create a websocket session for over an existing socket.
         *
         * In typical scenerios, a @c Session instance is created for a
         * @a socket is obtained by a call to
         * @c QTcpServer::nextPendingConnection().  The session instance is
         * configured to get notified when data is available and performs the
         * websocket handshake once a full HTTP request is received.
         *
         * @note Since the peer initiates the websocket handshake and the server
         *  is passive until the handshake compeletes, it is possible to create
         *  a session and then connect the socket.
         */
        Session ( QTcpSocket * socket, QObject * parent=0 );

        /*!
         * @warning The websocket closing handshake and TCP shutdown are not
         *  performed automatically.  Invoke the @c close() and @c shutdown()
         *  methods unless you want the same effect as @c kill().
         */
        virtual ~Session();

        /* methods. */
    public:
        /*!
         * @brief Automatically respond to websocket heartbeat messages.
         *
         * This function is a convenience wrapper for connecting the @c pinged()
         * signal and @c pong() slot.
         */
        void autopong ();

        /*!
         * @brief Send a UTF-8 encoded text message over the wire.
         * @param data Pointer to first byte of UTF-8 text to send.
         * @param size Number of bytes of data to send (length of UTF-8 string
         *  in @e bytes).
         *
         * Send a complete message over the wire.
         *
         * @warning The websocket specification requires implementation to drop
         *  the connection if invalid UTF-8 text is received over the wire.  The
         *  peer may decide to drop the connection if it stricly enforces the
         *  specification and you send any invalid UTF-8 text.  To transfer text
         *  in arbitrary encodings, use the @c senddata() method instead (and,
         *  by all means, send some prefix that explicitly and unambiguously
         *  identifies which encoding the text is in.
         */
        void sendtext ( const void * data, quint64 size );

        /*!
         * @brief UTF-8 encode and send a string.
         * @param payload Text to send as a websocket message.
         *
         * Send a complete message over the wire.
         *
         * @note This is the preferred way to send text over the wire, unless
         *  text to send is already encoded as UTF-8.
         */
        void sendtext ( const QString& payload );

        /*!
         * @brief Send a UTF-8 encoded text message over the wire.
         * @param payload Text to send as a websocket message.
         *
         * Send a complete message over the wire.
         *
         * @warning The websocket specification requires implementation to drop
         *  the connection if invalid UTF-8 text is received over the wire.  The
         *  peer may decide to drop the connection if it stricly enforces the
         *  specification and you send any invalid UTF-8 text.  To transfer text
         *  in arbitrary encodings, use the @c senddata() method instead (and,
         *  by all means, send some prefix that explicitly and unambiguously
         *  identifies which encoding the text is in.
         */
        void sendtext ( const QByteArray& payload );

        /*!
         * @brief Send a binary message over the wire.
         * @param data Pointer to first byte of binary data to send.
         * @param size Number of bytes of data to send.
         *
         * Send a complete message over the wire.
         *
         * @warnings Some websocket implementations do not support binary
         *  messages.  Make sure the peer can accept binary messages before
         *  sending them.  This can be achieved by negotiating capacities after
         *  the handshake.
         */
        void senddata ( const void * data, quint64 size );

        /*!
         * @brief Send a binary message over the wire.
         * @param payload Entire message contents to send over the wire.
         *
         * Send a complete message over the wire.
         *
         * @warnings Some websocket implementations do not support binary
         *  messages.  Make sure the peer can accept binary messages before
         *  sending them.  This can be achieved by negotiating capacities after
         *  the handshake.
         */
        void senddata ( const QByteArray& payload );

        /* signals. */
    signals:
        void debug ( const QString& status );

        /*!
         * @brief HTTP request received, handshake in progress.
         *
         * This signal is fired after a complete HTTP request is received and
         * before the websocket handshake is checked.  It is mostly useful for
         * providing status updates (tell the use the handshake is in progress).
         *
         * @note This signal may be emitted multiple times if there is websocket
         *  protocol version negotiation between the client and the server.
         */
        void request ();

        /*!
         * @brief Invalid handshake request, websocket connection dropped.
         *
         * This signal is fired when the HTTP request was an invalid websocket
         * handshake.  The websocket specification requires to drop the TCP
         * connection if the handshake is invalid.  Therefore, the connection is
         * killed before the signal is fired.
         */
        void invalid ();

        /*!
         * @brief Hanshake complete, websocket connection upgrade successful.
         *
         * This signal is fired when the websocket HTTP handshake is complete
         * and the connection is effectively upgraded.  Websocket messages may
         * be received after this signal is fired.
         */
        void upgrade ();

        /*!
         * @brief Websocket close frame received, peer won't send more data.
         *
         * This signal is fired when the websocket closing handshake is received
         * from the peer.  If @c close() has not been called and the TCP
         * connection is not broken, you may still send data over the wire.
         */
        void closed ();

        /*!
         * @brief Connection lost without 
         *
         * @note If the TCP connection fails after the closing handshake is
         *  received, this signal is not fired.
         */
        void killed ();

        /*!
         * @brief Received heartbeat from peer.
         *
         * If @c autopong() has not been set, the application should reply with
         * @c pong(data,size) when convenient.  Reply to make sure the peer
         * doesn't disconnect after not receiving the response heartbeat for a
         * long time.
         */
        void pinged ( const void * data, quint64 size );

        /*!
         * @brief Received response to heartbeat from peer.
         *
         * @note The websocket specification requires the application to check
         *  that the data is identical to the data sent in a ping request.  If
         *  the application wants to strictly enforce the specification, it
         *  should keep track of sent ping requests and compare the received
         *  pong with was was previously sent.
         */
        void ponged ( const void * data, quint64 size );

        /*!
         * @brief Received data over the wire, frames will follow.
         */
        void newmessage ();

        /*!
         * @brief Message frame header received.
         * @param size Frame size (in @e bytes).
         */
        void newframe ( quint64 size );

        /*!
         * @brief Received (partial) text frame data.
         * @param data Pointer to first byte of data received.
         * @param size Amount of data received, in @e bytes.
         *
         * This function may be called multiple times, even for a single frame.
         *
         * The websocket specification requries this data to be UTF-8 encoded
         * text.  Since this may be a partial frame and may stop at arbitrary
         * positions in the string, the last byte(s) may contain a partial
         * character.  The string content is therefore not decoded.
         *
         * @note If the application wishes to strictly enforce the websocket
         *  specification, it should @c kill() the connection when it encounters
         *  UTF-8 decoding errors.
         */
        void text ( const void * data, quint64 size );

        /*!
         * @brief Received (partial) binary frame data.
         * @param data Pointer to first byte of data received.
         * @param size Amount of data received, in @e bytes.
         *
         * This function may be called multiple times, even for a single frame.
         */
        void data ( const void * data, quint64 size );

        /*!
         * @brief Message frame completely received.
         */
        void endframe ();

        /*!
         * @brief All message frames completely received.
         */
        void endmessage ();

        /* slots. */
    public slots:
        /*!
         * @brief Emit websocket heartbeat.
         *
         * This slot is compatible with the @c QTimer::timeout() signal. You can
         * configure the websocket for automatic heartbeats by using a @c QTimer
         * instance.  To send data along with the ping, you need to create a
         * slot in some other object which then invokes the
         * @c ping(const void*,quint64) overload.
         *
         * @note You should expect an eventual @c ponged() call.
         */
        void ping ();

        /*!
         * @brief Emit websocket heartbeat.
         *
         * @note You should expect an eventual @c ponged() call with an echo of
         *  the ping data sent in this call.  Strictly speaking, you should
         *  verify that the peer echoes the data byte for byte.
         */
        void ping ( const void * data, quint64 size );

        /*!
         * @brief Respond to websocket heartbeat.
         *
         * This slot is compatible with the @c QTimer::timeout() signal. You can
         * configure the websocket for automatic (uni-directional) heartbeats by
         * using a @c QTimer instance.  To send data along with the pong, you
         * need to create a slot in some other object which then invokes the
         * @c pong(const void*,quint64) overload.
         *
         * @note It is perfectly valid to send unsollicited pongs to implement
         *  uni-directional heartbeats.
         */
        void pong ();

        /*!
         * @brief Respond to websocket heartbeat.
         *
         * This slot is compatible with the @c pinged() signal.  To set up
         * automatic responses to ping messages, connect the two slots.  Call
         * the @c autoping() method to have this done for you.
         *
         * @note It is perfectly valid to send unsollicited pongs to implement
         *  uni-directional heartbeats.
         */
        void pong ( const void * data, quint64 size );

        /*!
         * @brief Indicate that we won't send any more frames.
         *
         * @note This does not close the socket and you may keep receiving data
         *  from the peer for an indeterminate period of time.  To initiate full
         *  TCP shutdown, call @c shutdown().
         */
        void close ();

        /*!
         * @brief Indicate that we won't send any more frames.
         *
         * @note This does not close the socket and you may keep receiving data
         *  from the peer for an indeterminate period of time.  To initiate full
         *  TCP shutdown, call @c shutdown().
         */
        void close ( const void * data, quint64 size );

        /*!
         * @brief Initiate full TCP shutdown.
         *
         * @warning The session instance is configured to delete on completion
         *  of disconnect operator.
         */
        void shutdown ();

        /*!
         * @brief Forcibly close the TCP socket.
         *
         * @warning No websocket close frame or TCP shutdown is performed.  Use
         *  this only under extreme server circumstances or when it is known
         *  that connection state has been compromised.
         */
        void kill ();

    private slots:
        // Handle data received on socket.  Feed appropriate low-level parser.
        void consume ();

        /* class methods. */
    private:
        // Callbacks registered with low-level incremental parsers.
        static void new_message ( ::ws_iwire * backend );
        static void end_message ( ::ws_iwire * backend );
        static void new_frame ( ::ws_iwire * backend, uint64 size );
        static void end_frame ( ::ws_iwire * backend );
        static void accept_ibound_content (
            ::ws_iwire * backend, const void * data, uint64 size );
        static void accept_obound_content (
            ::ws_owire * backend, const void * data, uint64 size );
    };

}

#endif /* SESSION_HPP */
