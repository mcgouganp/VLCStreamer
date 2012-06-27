// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#include "VlcEncoder.h"
#include <QFileInfo>
#include <QDebug>
#include <QProcess>

void VlcEncoder::run()
{
	QFileInfo	info(_scriptFilename);

	QProcess	encoder;

	//encoder.setWorkingDirectory("\"" + info.path() + "\"");
	encoder.setStandardErrorFile(info.path() + "/errVLC.log");
	encoder.setStandardOutputFile(info.path() + "/logVLC.log");

	QString cmd = "\"" + info.filePath() + "\"";
	//qDebug() << "Trying to start " << cmd;
	encoder.start(cmd, QIODevice::ReadOnly);

	if(encoder.waitForStarted() == false) {
		qDebug() << "encoder starting problem " << QString::number(encoder.exitCode()) << QString::number(encoder.exitStatus()) << QString::number(encoder.error());
		return;
	}

	if(encoder.waitForFinished(-1) == false) {
		qDebug() << "encoder finishing problem " << QString::number(encoder.exitCode()) << QString::number(encoder.exitStatus()) << QString::number(encoder.error());
	}

	//qDebug() << "Script is done " << QString::number(encoder.exitCode()) << QString::number(encoder.exitStatus()) << QString::number(encoder.error());

	// Log result

	// Mark complete
	if(encoder.exitCode() == 0) {
		QFile	complete(info.path() + "/complete.txt");
		if(complete.open(QIODevice::WriteOnly)) {
			complete.close();
		}
	}
}

