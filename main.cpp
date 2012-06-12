#include <QCoreApplication>
#include <QSocketNotifier>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "VlcStreamerApp.h"


static int	_sigFd[2];


void _Handler(int /*sig*/)
{
	char	a;
	
	if(::write(_sigFd[0], &(a = 1), sizeof(a)) != sizeof(a)) {
		_exit(1);
	}
}


int main(int argc, char **argv)
{
	int					rtn;
	QSocketNotifier		*snSignal;
	struct sigaction	act, oact;
	
	QCoreApplication	app(argc, argv);

	socketpair(AF_UNIX, SOCK_STREAM, 0, _sigFd);
	snSignal = new QSocketNotifier(_sigFd[1], QSocketNotifier::Read);
	QObject::connect(snSignal, SIGNAL(activated(int)), &app, SLOT(quit()));
	
	act.sa_handler = _Handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, &oact);
	sigaction(SIGTERM, &act, 0);

	VlcStreamerApp	streamer;

	if(streamer.Setup()) {
		QObject::connect(&app, SIGNAL(aboutToQuit()), &streamer, SLOT(Stop()));
		rtn = app.exec();
	} else {
		rtn = 1;
	}

	sigaction(SIGINT, &oact, 0);
	sigaction(SIGTERM, &oact, 0);

	return rtn;
}