#include "VlcStreamerService.h"
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <QSocketNotifier>


VlcStreamerService::VlcStreamerService(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "Vlc Streamer Service")
{
	setServiceDescription("");
	socketpair(AF_UNIX, SOCK_STREAM, 0, _sigFd);
}


void VlcStreamerService::start()
{
	QSocketNotifier		*snSignal;
	struct sigaction	act;

	QCoreApplication *app = application();

	snSignal = new QSocketNotifier(_sigFd[1], QSocketNotifier::Read);
	QObject::connect(snSignal, SIGNAL(activated(int)), app, SLOT(quit()));
	
	act.sa_handler = _Handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, 0);
	sigaction(SIGTERM, &act, 0);

	_app = new VlcStreamerApp();
	if(_app->Setup()) {
		QObject::connect(app, SIGNAL(aboutToQuit()), _app, SLOT(Stop()));
		return;
	}
	app->quit();
}


void VlcStreamerService::stop()
{
	struct sigaction	act;

	act.sa_handler = SIG_DFL;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGINT, &act, 0);
	sigaction(SIGTERM, &act, 0);

}


// ---------------------------------------------------------------


int VlcStreamerService::_sigFd[2];


void VlcStreamerService::_Handler(int /*sig*/)
{
	char	a;
	
	if(::write(_sigFd[0], &(a = 1), sizeof(a)) != sizeof(a)) {
		_exit(1);
	}
}


