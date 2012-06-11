#include "VlcEncoder.h"
#include <QFileInfo>
#include <QDebug>
#include <QProcess>

void VlcEncoder::run()
{
	QFileInfo	info(_scriptFilename);

	QProcess	encoder;

	encoder.setWorkingDirectory(info.completeBaseName());
	encoder.setStandardErrorFile("errVLC.log");
	encoder.setStandardOutputFile("logVLC.log");
	qDebug() << "Trying to start " << _scriptFilename;
	encoder.start(_scriptFilename);

	encoder.waitForFinished(-1);

	qDebug() << "Script is done " << QString::number(encoder.exitCode());

	// Log result
	if(encoder.error() == 0) {
		QFile	complete(info.path() + "/complete.txt");
		if(complete.open(QIODevice::WriteOnly)) {
			complete.close();
		}
	}
}

