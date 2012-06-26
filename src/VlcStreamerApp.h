#ifndef _VlcStreamerApp_h
#define _VlcStreamerApp_h

#include <QProcess>
#include "VlcStreamerFileSystem.h"

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
	QString	TempDir() const			{ return _tempDir; }
	VlcStreamerFileSystem &Movies()	{ return *_fs; }

public slots:
	void	Stop();

private:
	static const QString	_AppName;
	static const QString	_Org;
	static const QString	_Domain;

	static VlcStreamerApp	*_instance;

	unsigned				_listenPort;
	QString					_documentRoot;
	QString					_queueDir;
	QString					_homeDir;
	QString					_drivesDir;
	QString					_storeDir;
	QString					_tempDir;
	QProcess				*_mdns;
	VlcStreamerFileSystem	*_fs;
};

#endif	// _VlcStreamerApp_h
