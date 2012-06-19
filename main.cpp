#include "VlcStreamerService.h"


int main(int argc, char **argv)
{
	VlcStreamerService	service(argc, argv);
	return service.exec();
}
