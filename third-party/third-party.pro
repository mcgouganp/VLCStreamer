# Copyright (C) 2012 Paul McGougan
# 
# Distributed under the GNU General Public License 2.0. (See acommpanying file
# LICENSE or copy at http://www.gnu.org/licenses/).

TEMPLATE = subdirs

include(../common.pri)

SRC = ftp://ftp.trolltech.com/qt/solutions/lgpl/qtservice-2.6_1-opensource.tar.gz
FILE = $$basename(SRC)
DIR = $$system(basename $$SRC .tar.gz)

getfile.target = $$FILE
getfile.commands = wget $$SRC

untarfile.target = $$DIR
untarfile.commands = tar zxf $$FILE
untarfile.depends = getfile

force.target = FORCE
force.depends = untarfile

QMAKE_EXTRA_TARGETS += getfile untarfile force

QMAKE_DISTCLEAN += $$FILE $$DIR
