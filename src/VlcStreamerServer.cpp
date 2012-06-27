// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#include "VlcStreamerServer.h"
#include "VlcStreamerConnection.h"


VlcStreamerServer::VlcStreamerServer(quint16 port, QObject *parent) : QTcpServer(parent)
{
	listen(QHostAddress::Any, port);
}


VlcStreamerServer::~VlcStreamerServer()
{
	close();
}


void VlcStreamerServer::incomingConnection(int socket)
{
	VlcStreamerConnection	*conn = new VlcStreamerConnection(this);
	conn->setSocketDescriptor(socket);
	emit newConnection();
}