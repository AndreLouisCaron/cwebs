// Copyright(c) Andre Caron <andre.l.caron@gmail.com>, 2011
//
// This document is covered by the an Open Source Initiative approved license. A
// copy of the license should have been provided alongside this software package
// (see "LICENSE.txt"). If not, terms of the license are available online at
// "http://www.opensource.org/licenses/mit".

// http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-17

#include "qwsserver.hpp"
#include "qwssession.hpp"

#include <cstdlib>
#include <algorithm>
#include <locale>
#include <sstream>

#include <QDebug>
#include <QtGui>
#include <QtNetwork>

namespace qws {

    Server::Server ( QWidget * parent )
        : QDialog(parent), myStatus(), myCancel(tr("Quit")), myServer()
    {
          // Configure UI.
        {
            QHBoxLayout *const inner = new QHBoxLayout();
            inner->addStretch(1);
            inner->addWidget(&myCancel);
            inner->addStretch(1);
            QVBoxLayout *const outer = new QVBoxLayout();
            outer->addWidget(&myStatus);
            outer->addLayout(inner);
            setLayout(outer);
        }
        setWindowTitle(tr("Qt / Web Socket Server"));
        
          // Close everything when asked to.
        myCancel.setAutoDefault(false);
        connect(&myCancel, SIGNAL(clicked()), this, SLOT(close()));
        
          // Configure server.
        connect(&myServer, SIGNAL(newConnection()), this, SLOT(accept()));
        
          // Start accepting connections.
        start();
    }

    void Server::start ()
    {
          // Start accepting connections.
        const QHostAddress host = QHostAddress::Any;
        const quint16 port = 1234;
        if ( myServer.listen(host, port) ) {
            myStatus.setText(tr("Web socket server online!"));
        }
        else
        {
            QMessageBox::critical(this, tr("Qt / Web Socket Server"),
                tr("Unable to start the server: %1.")
                .arg(myServer.errorString()));
            close();
        }
    }

    void Server::accept ()
    {
          // Accept and wrap connection.
        Session *const session = new Session(
            myServer.nextPendingConnection(), 0);
          // Get notified when client session ends.
        QObject::connect(session, SIGNAL(destroyed()), this, SLOT(finished()));
    }

}
