#include <iostream>
#include <QCoreApplication>
#include <QSocketNotifier>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "VlcStreamerApp.h"


static const QString	_AppName	= "VlcStreamer";
static const QString	_Org		= "gallandro.net";

static int	_sigFd[2];


void _Handler(int sig)
{
	char	a;
	::write(_sigFd[0], &(a = 1), sizeof(a));
}


int main(int argc, char **argv)
{
	int					rtn;
	QSocketNotifier		*snSignal;
	struct sigaction	act, oact;
	
	QCoreApplication	app(argc, argv);

	app.setApplicationName(_AppName);
	app.setOrganizationName(_Org);

	std::cerr << "PID: " << app.applicationPid() << std::endl;
	
	socketpair(AF_UNIX, SOCK_STREAM, 0, _sigFd);
	snSignal = new QSocketNotifier(_sigFd[1], QSocketNotifier::Read);
	QObject::connect(snSignal, SIGNAL(activated(int)), &app, SLOT(quit()));
	
	act.sa_handler = _Handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, &oact);
	sigaction(SIGTERM, &act, 0);

	std::cerr << "Starting" << std::endl;

	VlcStreamerApp	streamer;
	QObject::connect(&app, SIGNAL(aboutToQuit()), &streamer, SLOT(Stop()));

	std::cerr << "Running exec" << std::endl;

	rtn = app.exec();

	sigaction(SIGINT, &oact, 0);
	sigaction(SIGTERM, &oact, 0);

	std::cerr << "Exiting" << std::endl;

	return rtn;
}