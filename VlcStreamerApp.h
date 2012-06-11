#ifndef _VlcStreamerApp_h
#define _VlcStreamerApp_h

#include <QProcess>
#include "VlcStreamerServer.h"

class VlcStreamerApp : public QObject {
Q_OBJECT
public:
	VlcStreamerApp(QObject *parent = 0);

public slots:
	void	Stop();

private:
	QProcess	*_mdns;
};

#endif	// _VlcStreamerApp_h
