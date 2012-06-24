#ifndef _VlcStreamerService_h
#define _VlcStreamerService_h

#include "VlcStreamerApp.h"
#include "qtservice.h"

class VlcStreamerService : public QtService<QCoreApplication> {
public:
	VlcStreamerService(int argc, char **argv);

	void	start();
	void	stop();

protected:
	static int	_sigFd[2];

	VlcStreamerApp		*_app;

	static void _Handler(int sig);
};

#endif	// _VlcStreamerService_h
