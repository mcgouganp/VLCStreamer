# Copyright (C) 2012 Paul McGougan
# 
# Distributed under the GNU General Public License 2.0. (See acommpanying file
# LICENSE or copy at http://www.gnu.org/licenses/).

win32 {
	QMAKE_DISTCLEAN = /q
} else {
	QMAKE_DISTCLEAN = -r
}
