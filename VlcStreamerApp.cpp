#include "VlcStreamerApp.h"


static const QString	_listenPort	= "8124";


VlcStreamerApp::VlcStreamerApp(QObject *parent) : QObject(parent)
{
	QStringList		args;
	QString			temp;

	new VlcStreamerServer(_listenPort.toUInt(), this);

	_mdns = new QProcess(this);

	temp = "http=";
	temp += _listenPort;

	args << "-s" << "tassie.braintree.com.au" << "_hs-vlcstream._tcp" << _listenPort << temp << "version=2" << "minVersion=1" << "platform=unix";
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