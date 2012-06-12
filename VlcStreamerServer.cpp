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