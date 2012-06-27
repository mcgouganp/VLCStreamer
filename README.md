VLCStreamer
===========

This is a QT-based server for the VLCStreamerClient. It runs as a daemon process (i.e. no GUI). It is written using the QT framework. Since it is QT-based, there has been some thought given to making it multiplatform compatible, however it is primarily targetted to Linux installations.


Prerequisites
-------------
Qt-Devel is required to compile/build install this package.
If you want to use the included SPEC file to build a binary RPM of this package you will also need rpmbuild installed.

To run the software you will require the following software to be installed and in your path:
* avahi-publish-service - Provided by avahi-tools on Fedora
* vlc 2.0.0 or later - Provided by vlc on Fedora


Compiling
---------
To build you should follows steps like this:
qmake
make
make install

If you want to install from a RPM to allow software package management of this software you can instead run:
rpmbuild -ta vlcstreamer.tar.gz
As this package includes a SPEC file, this will build a binary RPM that can be used to install the software instead.


Installing
----------


Running
-------
This is written as a service, so by default it is not executed in the foreground. To override this and run in the foreground use:
    vlcstreamer -e


Configuring
-----------
The configuration file is written on first run (or installation if the RPM is used). Configuration is stored in $USER/.config/Gallandro/VlcStreamer.conf.
It is a simple "key=value" format. The currently supported keys are:
* port - Advertises and listens on this port number (defaults to 8124)
* home - The directory mapped to VlcStreamer's "home" request
* media - The directory mapped to VlcStreamer's "media" request
* store - The directory that the program's filesystem, which contains transcoded data/log files, is rooted on.


Licensing
---------
Copyright (C) 2012 Paul McGougan

Distributed under the GNU General Public License 2.0. (See acommpanying file
LICENSE or copy at http://www.gnu.org/licenses/).
