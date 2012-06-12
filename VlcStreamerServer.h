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
