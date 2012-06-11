#ifndef _VlcStreamerApp_h
#define _VlcStreamerApp_h

#include <QProcess>
#include "VlcStreamerServer.h"

class VlcStreamerApp : public QObject {
Q_OBJECT
public:
	static VlcStreamerApp	*Instance()	{ return _instance; }

	VlcStreamerApp(QObject *parent = 0);

	QString	HomeDir() const		{ return _homeDir; }
	QString	DrivesDir() const	{ return _drivesDir; }

public slots:
	void	Stop();

private:
	static VlcStreamerApp	*_instance;

	QString		_documentRoot;
	QString		_homeDir;
	QString		_drivesDir;
	QProcess	*_mdns;
};

#endif	// _VlcStreamerApp_h
