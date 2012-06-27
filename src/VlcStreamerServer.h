// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#ifndef _VlcStreamerServer_h
#define _VlcStreamerServer_h

#include <signal.h>
#include <QTcpServer>

class VlcStreamerServer : public QTcpServer {
	Q_OBJECT
public:
	VlcStreamerServer(quint16 port, QObject *parent);
	virtual ~VlcStreamerServer();

	void incomingConnection(int socket);
};

#endif	// _VlcStreamerServer_h
