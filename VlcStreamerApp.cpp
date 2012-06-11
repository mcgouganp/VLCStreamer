#include "VlcStreamerApp.h"
#include <QDir>
#include <QHostInfo>
#include "VlcEncodingSystem.h"

static const QString	_listenPort	= "8124";

//QString VlcStreamerApp::DocumentRoot

VlcStreamerApp::VlcStreamerApp(QObject *parent) : QObject(parent)
{
	_instance = this;

	///.Hobbyist_Software/VLC_Streamer/Root/_Queue
	static const QString	OrgString = ".Hobbyist_Software";
	static const QString	AppString = "VLC_Streamer";
	static const QString	RootString = "Root";
	static const QString	QueueString = "_Queue";

	QStringList		args;
	QString			temp;

	QDir	dir(QDir::home());
	_documentRoot = dir.path();
	_homeDir = QDir::homePath() + "/Videos";
	_drivesDir = "/media";

	if(dir.exists(OrgString) == false) {
		dir.mkdir(OrgString);
	}
	dir.cd(OrgString);
	if(dir.exists(AppString) == false) {
		dir.mkdir(AppString);
	}
	dir.cd(AppString);
	if(dir.exists(RootString) == false) {
		dir.mkdir(RootString);
	}
	dir.cd(RootString);
	if(dir.exists(QueueString) == false) {
		dir.mkdir(QueueString);
	}

	new VlcEncodingSystem(this);

	new VlcStreamerServer(_listenPort.toUInt(), this);

	_mdns = new QProcess(this);

	temp = "http=";
	temp += _listenPort;

	args << "-s" << QHostInfo::localHostName() << "_hs-vlcstream._tcp" << _listenPort << temp << "version=2" << "minVersion=1" << "platform=unix";
	qDebug() << "Before running MDNS";
	_mdns->start("avahi-publish-service", args);
	qDebug() << "MDNS is running on " << _mdns->pid();
}


void VlcStreamerApp::Stop()
{
	qDebug() << "Terminating MDNS";
	_mdns->terminate();
	qDebug() << "MDNS State " << _mdns->state();
	_mdns->waitForFinished();
	qDebug() << "MDNS State " << _mdns->state();
}


// ------------------------------------------------------


VlcStreamerApp	*VlcStreamerApp::_instance = 0;

