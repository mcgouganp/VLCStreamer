#include "VlcEncodingSystem.h"
#include <QDir>
#include <QFile>
#include <QFileSystemWatcher>
#include <QThreadPool>
#include <QVariantMap>
#include <qjson/parser.h>
#include "VlcEncoder.h"
#include "VlcStreamerApp.h"

#include <QDebug>

VlcEncodingSystem::VlcEncodingSystem(QObject *parent) : QObject(parent)
{
	QFileSystemWatcher	*watcher = new QFileSystemWatcher(this);

	connect(watcher, SIGNAL(directoryChanged(const QString &)), SLOT(OnChange(const QString &)));

	watcher->addPath(VlcStreamerApp::Instance()->QueueDir());
}



void VlcEncodingSystem::OnChange(const QString &arg)
{
	QDir	dir(arg);
	QStringList	matchingFiles;
	matchingFiles << "*.params.txt";
	QFileInfoList	list = dir.entryInfoList(matchingFiles, QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) list.size(); ++i) {
		_DoEncode(list[i].filePath());
	}
}


void VlcEncodingSystem::_DoEncode(const QString &paramFile)
{
	QFileInfo	videoFile(QFileInfo(QFileInfo(paramFile).completeBaseName()).completeBaseName());

	QDir		dir(VlcStreamerApp::Instance()->DocumentRoot());
	QString		name;
	if(dir.exists(videoFile.completeBaseName())) {
		for(unsigned i = 1; i != ~0U; ++i) {
			if(dir.exists(videoFile.completeBaseName() + "_" + QString::number(i)) == false) {
				name = videoFile.completeBaseName() + "_" + QString::number(i);
				break;
			}
		}
	} else {
		name = videoFile.completeBaseName();
	}

	if(name.isEmpty() == false) {
		dir.mkdir(name);
		dir.cd(name);
		dir.rename(paramFile, "params.txt");

#ifdef Q_WS_WIN
		QFileInfo	scriptInfo(dir.path() + "/encode.bat");
#else
		QFileInfo	scriptInfo(dir.path() + "/encode.sh");
#endif
		QFile		script(scriptInfo.filePath());
		QFile		params(dir.path() + "/params.txt");

		if(params.open(QIODevice::ReadWrite)) {
			QString paramData = params.readAll();
			params.close();

			bool ok;
			QJson::Parser	parser;
			QVariantMap		map = parser.parse(paramData.toAscii(), &ok).toMap();
			if(ok && script.open(QIODevice::WriteOnly)) {
				QString	scriptData;
				//#!/bin/bash 
				//vlc -I dummy --ignore-config "/home/paulm/Videos/troops.avi" 
				//		'--sout=#transcode{vcodec=h264,soverlay,acodec=mp3,channels=2,venc=x264{profile=baseline,level=2.2,keyint=30,bframes=0,ref=1,nocabac},width=480,vb=200,ab=40,fps=25,deinterlace}:
				//				std{access=livehttp{seglen=10,index="/home/paulm/.Hobbyist_Software/VLC_Streamer/Root/troops/stream.m3u8",index-url=stream-###.ts},
				//				mux=ts{use-key-frames},
				//				dst="/home/paulm/.Hobbyist_Software/VLC_Streamer/Root/troops/stream-###.ts"}'
				//		vlc://quit
				scriptData = "#!/bin/bash\n";
				QString	temp = map.value("sout").toString();
				temp.replace("##dest##", dir.path());
				scriptData += "vlc -I dummy --ignore-config " + map.value("args").toString() + " \"" + map.value("file").toString() + "\" vlc://quit " + "'" + temp + "' -vvv\n";
				script.write(scriptData.toAscii());
				script.close();
				script.setPermissions(QFile::ReadUser | QFile::WriteUser | QFile::ExeUser);

				VlcEncoder *encoder = new VlcEncoder(scriptInfo.filePath());
				QThreadPool::globalInstance()->start(encoder);
			} else {
				qDebug() << "Not ok or script didn't open" << ok << script.error();
			}
		}
	}
}


