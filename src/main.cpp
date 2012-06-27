// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#include "VlcStreamerService.h"


int main(int argc, char **argv)
{
	VlcStreamerService	service(argc, argv);
	return service.exec();
}
