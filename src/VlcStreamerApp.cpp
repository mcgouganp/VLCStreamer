#include "VlcStreamerApp.h"
#include <QCoreApplication>
#include <QDir>
#include <QHostInfo>
#include <QSettings>
#include "Utils.h"
#include "VlcEncodingSystem.h"
#include "VlcStreamerServer.h"


VlcStreamerApp::VlcStreamerApp(QObject *parent) : QObject(parent)
{
	_instance = this;

	QCoreApplication	*appInstance = QCoreApplication::instance();

	appInstance->setOrganizationName(_Org);
	appInstance->setOrganizationDomain(_Domain);
	appInstance->setApplicationName(_AppName);
	appInstance->setApplicationVersion("0.1");

	QSettings	settings;

	if(settings.contains("home") == false) {
		settings.setValue("home", QDir::homePath() + "/Videos");
	}
	if(settings.contains("drives") == false) {
		settings.setValue("drives", "/media");
	}
	if(settings.contains("port") == false) {
		settings.setValue("port", 8124);
	}
	settings.sync();

	_homeDir = settings.value("home").toString();
	_drivesDir = settings.value("drives").toString();
	_listenPort = settings.value("port").toUInt();

}


bool VlcStreamerApp::Setup()
{
	static const QString	RootString = "Root";
	static const QString	QueueString = "_Queue";

	QCoreApplication	*appInstance = QCoreApplication::instance();
	QDir				dir(QDir::home());

	if(dir.exists("." + appInstance->organizationName()) == false && dir.mkdir("." + appInstance->organizationName()) == false) {
		return false;
	}
	dir.cd("." + appInstance->organizationName());
	if(dir.exists(appInstance->applicationName()) == false && dir.mkdir(appInstance->applicationName()) == false) {
		return false;
	}
	dir.cd(appInstance->applicationName());
	if(dir.exists(RootString) == false && dir.mkdir(RootString) == false) {
		return false;
	}
	dir.cd(RootString);
	_documentRoot = dir.path();
	if(dir.exists(QueueString) == false && dir.mkdir(QueueString) == false) {
		return false;
	}
	dir.cd(QueueString);
	_queueDir = dir.path();

	QDir	temp(QDir::temp());

	if(temp.exists(appInstance->applicationName()) == false && temp.mkdir(appInstance->applicationName()) == false) {
		return false;
	}
	temp.cd(appInstance->applicationName());
	_tempDir = temp.path();

	new VlcEncodingSystem(this);
	new VlcStreamerServer(_listenPort, this);
	_fs = new VlcStreamerFileSystem(this);

	QStringList args;
	args << "-s" << QHostInfo::localHostName() << "_hs-vlcstream._tcp" << QString::number(_listenPort) << ("http=" + QString::number(_listenPort)) << "version=3" << "minVersion=1" << "platform=unix";
	qDebug() << "Starting MDNS";
	_mdns = new QProcess(this);
	_mdns->start("avahi-publish-service", args);
	return _mdns->waitForStarted();
}


void VlcStreamerApp::Stop()
{
	qDebug() << "Terminating MDNS";
	_mdns->terminate();
	_mdns->waitForFinished();

	QDir	temp(_tempDir);
	DeleteDirectory(temp);
}


// ------------------------------------------------------


const QString	VlcStreamerApp::_Org		= "Gallandro";
const QString	VlcStreamerApp::_Domain		= "gallandro.net";
const QString	VlcStreamerApp::_AppName	= "VlcStreamer";

VlcStreamerApp	*VlcStreamerApp::_instance = 0;


