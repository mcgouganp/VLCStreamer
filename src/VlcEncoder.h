// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#ifndef _VlcEncoder_h
#define _VlcEncoder_h

#include <QRunnable>
#include <QString>
#include <QProcess>

class VlcEncoder : public QRunnable {
public:
	VlcEncoder(const QString &path) : _scriptFilename(path)  {}

	void run();

protected:
	QString	_scriptFilename;
};
#endif	// _VlcEncoder_h
