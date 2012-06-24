#include "VlcStreamerFileSystem.h"
#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include "Utils.h"
#include "VlcStreamerApp.h"


VlcStreamerFileSystem::VlcStreamerFileSystem(QObject *parent) : QObject(parent)
{
	QDir	dir(VlcStreamerApp::Instance()->DocumentRoot());
	QDir	queue(VlcStreamerApp::Instance()->QueueDir());

	QStringList	list = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
	list.removeAll(queue.dirName());
	for(unsigned i = 0; i != (unsigned) list.size(); ++i) {
		if(dir.cd(list.at(i))) {
			_ConvertedVideoInfo	info;
			if(_GetDirectoryInfo(dir, info)) {
				_cache.insert(dir.dirName(), info);
				dir.cdUp();
			} else {
				qDebug() << "Directory " << list.at(i) << "corrupt on startup, deleting";
				DeleteDirectory(dir);
				// This implicitly returns us up a dir level when deleting the current dir
			}
		} else {
			qDebug() << "Unable to see in directory" << (dir.path() + list.at(i));
		}
	}

	QFileSystemWatcher	*watcher = new QFileSystemWatcher(this);
	connect(watcher, SIGNAL(directoryChanged(const QString &)), SLOT(OnDirChange(const QString &)));
	watcher->addPath(VlcStreamerApp::Instance()->DocumentRoot());
}


QString	VlcStreamerFileSystem::ConvertedVideoInfo() const
{
	QVariantList	movies;
	for(QHash<QString, _ConvertedVideoInfo>::const_iterator	iter = _cache.constBegin(); iter != _cache.constEnd(); ++iter) {
		QVariantMap	movieInfo;
		movieInfo.insert("params", iter.value().params);
		movieInfo.insert("name", iter.key());
		movieInfo.insert("status", iter.value().status);
		movies.append(movieInfo);
	}
	QVariantMap	info;
	info.insert("movies", movies);
	QJson::Serializer	serializer;
	return serializer.serialize(info);
}


QString VlcStreamerFileSystem::MakeServerId() const
{
	static const QString src = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	QString	rtn;

	for(unsigned i = 0; i != 8; ++i) {
		rtn += src[(int) (random() % src.size())];
	}
	for(QHash<QString, _ConvertedVideoInfo>::const_iterator	iter = _cache.constBegin(); iter != _cache.constEnd(); ++iter) {
		if(rtn == iter.value().serverId) {
			rtn = MakeServerId();
		}
	}
	return rtn;
}


void VlcStreamerFileSystem::DeleteMovie(const QString &serverId)
{
	for(QHash<QString, _ConvertedVideoInfo>::const_iterator	iter = _cache.constBegin(); iter != _cache.constEnd(); ++iter) {
		if(serverId == iter.value().serverId) {
			QDir	dir(VlcStreamerApp::Instance()->DocumentRoot() + "/" + iter.key());
			_cache.remove(dir.dirName());
			DeleteDirectory(dir);
			break;
		}
	}
}


void VlcStreamerFileSystem::OnDirChange(const QString &arg)
{
	QDir	dir(arg);
	QDir	queue(VlcStreamerApp::Instance()->QueueDir());

	if(arg == VlcStreamerApp::Instance()->DocumentRoot()) {
		QStringList	list = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
		list.removeAll(queue.dirName());
		QSet<QString>	found = QSet<QString>::fromList(dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot));
		QSet<QString>	cached = QSet<QString>::fromList(_cache.keys());

		QSet<QString>	newDirs = found;
		newDirs.subtract(cached);
		for(QSet<QString>::const_iterator iter = newDirs.constBegin(); iter != newDirs.constEnd(); ++iter) {
			((QFileSystemWatcher *) sender())->addPath(dir.filePath(*iter));
		}

		QSet<QString>	deletedDirs = cached;
		deletedDirs.subtract(found);
		for(QSet<QString>::const_iterator iter = deletedDirs.constBegin(); iter != deletedDirs.constEnd(); ++iter) {
			((QFileSystemWatcher *) sender())->removePath(dir.filePath(*iter));
		}
	} else {
		if(dir.exists("complete.txt")) {
			((QFileSystemWatcher *) sender())->removePath(dir.path());
		}
		_ConvertedVideoInfo	info;
		if(_GetDirectoryInfo(dir, info)) {
			_cache[dir.dirName()] = info;
		}
	}
}


// -------------------------------------------------------------------


bool VlcStreamerFileSystem::_GetDirectoryInfo(const QDir &dir, _ConvertedVideoInfo &info)
{
	QJson::Parser	parser;
	QFile			params(dir.filePath("params.txt"));
	QString			paramText;

	if(params.open(QIODevice::ReadOnly)) {
		paramText = params.readAll();
		params.close();
		bool	ok;
		info.params = parser.parse(paramText.toAscii(), &ok).toMap();
		if(ok) {
			info.serverId = info.params["serverid"].toString();
			info.status = "not processed";
			if(dir.exists("complete.txt")) {
				info.status = "complete";
			} else if(dir.exists("stream.m3u8")) {
				QFile	index(dir.filePath("stream.m3u8"));
				if(index.open(QIODevice::ReadOnly)) {
					QString data = index.readAll();
					index.close();
					info.status = "segments-" + QString::number(data.count("stream-"));
				}
			}
			return true;
		}
	}
	return false;
}


