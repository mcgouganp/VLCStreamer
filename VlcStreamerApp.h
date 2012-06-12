#ifndef _VlcStreamerApp_h
#define _VlcStreamerApp_h

#include <QProcess>
#include "VlcStreamerServer.h"

class VlcStreamerApp : public QObject {
Q_OBJECT
public:
	static VlcStreamerApp	*Instance()	{ return _instance; }

	VlcStreamerApp(QObject *parent = 0);

	bool	Setup();

	QString	HomeDir() const			{ return _homeDir; }
	QString	DrivesDir() const		{ return _drivesDir; }
	QString	DocumentRoot() const	{ return _documentRoot; }
	QString	QueueDir() const		{ return _queueDir; }

public slots:
	void	Stop();

private:
	static const QString	_AppName;
	static const QString	_Org;
	static const QString	_Domain;

	static VlcStreamerApp	*_instance;

	QString		_documentRoot;
	QString		_homeDir;
	QString		_drivesDir;
	QString		_queueDir;
	QProcess	*_mdns;
};

#endif	// _VlcStreamerApp_h
