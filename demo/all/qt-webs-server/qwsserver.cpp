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
 * @file demo/all/qt-webs-server/qwsserver.cpp
 */

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
        QObject::connect(session, SIGNAL(destroyed(QObject*)),
                         this, SLOT(finished(QObject*)));
          // Get notified when client session updates its status.
        QObject::connect(session, SIGNAL(debug(const QString&)),
                         &myStatus, SLOT(setText(const QString&)));
    }

    void Server::finished ( QObject * object )
    {
        myStatus.setText(tr("Web socket server online!"));
    }

}
