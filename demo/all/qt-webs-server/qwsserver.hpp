#ifndef SERVER_HPP
#define SERVER_HPP

// Copyright(c) Andre Caron <andre.louis.caron@usherbrooke.ca>, 2011
//
// This document is covered by the an Open Source Initiative approved license. A
// copy of the license should have been provided alongside this software package
// (see "LICENSE.txt"). If not, terms of the license are available online at
// "http://www.opensource.org/licenses/mit".

#include <QtGui>
#include <QtNetwork>

namespace qws {

    class Session;

    class Server :
        public QDialog
    {
        Q_OBJECT;

        /* data. */
    private:
        QLabel myStatus;
        QPushButton myCancel;
        QTcpServer myServer;

        /* construction. */
    public:
        Server ( QWidget * parent=0 );

        /* methods. */
    private:
        void start ();

        /* slots. */
    private slots:
        void accept ();
        void finished ( QObject * object=0 );
    };

}

#endif /* SERVER_HPP */
