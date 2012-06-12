#include "VlcStreamerApp.h"
#include <QCoreApplication>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include "VlcEncodingSystem.h"


VlcStreamerApp::VlcStreamerApp(QObject *parent) : QObject(parent)
{
	static const QString	RootString = "Root";
	static const QString	QueueString = "_Queue";

	_instance = this;

	QCoreApplication	*appInstance = QCoreApplication::instance();

	unsigned		listenPort;
	QStringList		args;

	appInstance->setOrganizationName(_Org);
	appInstance->setOrganizationDomain(_Domain);
	appInstance->setApplicationName(_AppName);
	appInstance->setApplicationVersion("0.1");

	QSettings	settings;

	QStringList	ug = settings.allKeys();
	qDebug() << ug;

	_homeDir = settings.value("home", QDir::homePath() + "/Videos").toString();
	_drivesDir = settings.value("drives", "/media").toString();
	listenPort = settings.value("port", 8124).toUInt();

	QDir	dir(QDir::home());

	if(dir.exists("." + appInstance->organizationName()) == false) {
		dir.mkdir("." + appInstance->organizationName());
	}
	dir.cd("." + appInstance->organizationName());
	if(dir.exists(appInstance->applicationName()) == false) {
		dir.mkdir(appInstance->applicationName());
	}
	dir.cd(appInstance->applicationName());
	if(dir.exists(RootString) == false) {
		dir.mkdir(RootString);
	}
	dir.cd(RootString);
	if(dir.exists(QueueString) == false) {
		dir.mkdir(QueueString);
	}
	_documentRoot = dir.path();


	new VlcEncodingSystem(this);

	new VlcStreamerServer(listenPort, this);

	_mdns = new QProcess(this);

	args << "-s" << QHostInfo::localHostName() << "_hs-vlcstream._tcp" << QString::number(listenPort) << ("http=" + QString::number(listenPort)) << "version=2" << "minVersion=1" << "platform=unix";
	qDebug() << "Starting MDNS";
	_mdns->start("avahi-publish-service", args);
	qDebug() << "MDNS is running on " << _mdns->pid();
}


bool VlcStreamerApp::Setup()
{
	return true;
}


void VlcStreamerApp::Stop()
{
	qDebug() << "Terminating MDNS";
	_mdns->terminate();
	_mdns->waitForFinished();
}


// ------------------------------------------------------


const QString	VlcStreamerApp::_AppName	= "VlcStreamer";
const QString	VlcStreamerApp::_Org		= "Gallandro";
const QString	VlcStreamerApp::_Domain		= "gallandro.net";

VlcStreamerApp	*VlcStreamerApp::_instance = 0;


