#include "VlcStreamerConnection.h"
#include <QDateTime>
#include <QFile>
#include <qjson/parser.h>
#include <stdlib.h>
#include "VlcStreamerApp.h"


VlcStreamerConnection::VlcStreamerConnection(QObject *parent) : QTcpSocket(parent)
{
	qDebug("VlcStreamerConnection %p: connected", this);

	connect(&_readTimer, SIGNAL(timeout()), SLOT(_RecvTimeout()));
	connect(this, SIGNAL(readyRead()), SLOT(_RecvData()));
	connect(this, SIGNAL(disconnected()), SLOT(_Disconnected()));
	_readTimer.setSingleShot(true);
	_readTimer.start(10000);
}


VlcStreamerConnection::~VlcStreamerConnection()
{
}



// ---------------------------------------------------


void VlcStreamerConnection::_RecvTimeout()
{
	qDebug("VlcStreamerConnection %p: read timeout", this);
	write("HTTP/1.1 408 request timeout\r\nConnection: close\r\n\r\n408 request timeout\r\n");
	disconnectFromHost();
}


void VlcStreamerConnection::_RecvData()
{
	QByteArray	data;

	while(canReadLine()) {
		data = readLine();
		if(_request.AddLine(data) == false) {
			qDebug("VlcStreamerConnection %p: data error", this);
			_Response	response;
			response.Status(400).ResponsePhrase("Bad Request").Data("Bad Request");
			write(response.ToByteArray());
			disconnectFromHost();
		} else if(_request.IsComplete()) {
			_readTimer.stop();
			_Respond();
			_request.Clear();
		}
	}
}


void VlcStreamerConnection::_Disconnected()
{
	qDebug("VlcStreamerConnection %p: disconnected", this);
	close();
	_readTimer.stop();
}


void VlcStreamerConnection::_DeleteDirectory(QDir &dir)
{
	QFileInfoList	files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) files.size(); ++i) {
		dir.remove(files.at(i).fileName());
	}
	QFileInfoList	subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) subdirs.size(); ++i) {
		dir.cd(subdirs.at(i).fileName());
		_DeleteDirectory(dir);
	}
	QString current = dir.dirName();
	dir.cdUp();
	dir.rmdir(current);
}


QString VlcStreamerConnection::_ServerId()
{
	static const QString src = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	QString	rtn;

	for(unsigned i = 0; i != 8; ++i) {
		rtn += src[(int) (random() % src.size())];
	}
	return rtn;
}


void VlcStreamerConnection::_Respond()
{
	_Response	response;

	qDebug() << _request.Url().path();
	if(_request.Url().path() == "/secure") {
		QMultiMap<QByteArray, QByteArray>	map;

		QList<QPair<QString, QString> >		items;
		items = _request.Url().queryItems();
		qDebug() << "Items:" << items.size() << items;
		for(unsigned i = 0; i != (unsigned) items.size(); ++i) {
			map.insert(items.at(i).first.toAscii(), items.at(i).second.toAscii());
		}

		QByteArray	data;
		if(map.contains("command")) {
			QByteArray	command;
			command = map.value("command");
			if(command == "movies") {
				QVector<_VideoInfo>	info;
				info = _GetConvertedVideoInfo();
				data = "{\"" + command + "\":[";
				for(unsigned i = 0; i != (unsigned) info.size(); ++i) {
					if(i) {
						data += ",";
					}
					data += "{\"params\":" + info[i].params + ",\"name\":\"" + info[i].name + "\",\"status\":\"" + info[i].status + "\"}";
				}
				data += "]}";
				response.Data(data);
			} else if(command == "create") {
				QDir	dir(VlcStreamerApp::Instance()->DocumentRoot());
				QFileInfo	src(map.value("file"));
				QFile	params(dir.path() + "/_Queue/" + src.fileName() + ".params.txt");
				if(params.open(QIODevice::WriteOnly)) {
					data = "{";
					QMapIterator<QByteArray, QByteArray>	iter(map);
					while(iter.hasNext()) {
						QByteArray	name, value;
						name = iter.peekNext().key();
						value = iter.next().value();
						data += "\"" + name.replace("\"", "\\\"") + "\":\"" + value.replace("\"", "\\\"") + "\",";
					}
					data += QByteArray("\"serverid\":\"") + _ServerId().toAscii() + "\"";
					data += "}";
					params.write(data);
					params.close();
				} else {
					qDebug() << "Couldn't open params file for writing" << params.fileName();
				}
				response.Data("Encoding");

			} else if(command == "getpath" || command == "browse") {
				QString	path;
				if(command == "getpath") {
					if(map.value("dir") == "...drives...") {
						path = VlcStreamerApp::Instance()->DrivesDir();
					} else {
						path = VlcStreamerApp::Instance()->HomeDir();
					}
				} else {
					path = map.value("dir");
				}
				QDir	dir(path);
				QFileInfoList	list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);
				data = "{";
				data += "\"files\":[";
				if(list.isEmpty() == false) {
					for(unsigned i = 0; i != (unsigned) list.size(); ++i) {
						if(i) {
							data += ",";
						}
						data += "{\"name\":\"" + list.at(i).fileName() + "\",\"path\":\"" + list.at(i).filePath() + "\",\"type\":\"" + (list.at(i).isDir() ? "dir" : "file") + "\"}";
					}
				}
				data += "],";
				data += "\"root\":\"" + path + "\"}";
				response.Data(data);
			} else if(command == "delete") {
				QVector<_VideoInfo>	info;
				info = _GetConvertedVideoInfo();
				
				for(unsigned i = 0; i != (unsigned) info.size(); ++i) {

					bool ok;
					QJson::Parser	parser;
					QVariantMap		result = parser.parse(info[i].params, &ok).toMap();
					if(ok) {
						if(result["serverid"].toByteArray() == map.value("id")) {
							QDir	dir(VlcStreamerApp::Instance()->DocumentRoot() + "/" + info[i].name);
							qDebug() << "Found matching serverId, deleting directory " << dir.path();	
							_DeleteDirectory(dir);
							break;
						}
					} else {
						qDebug() << "Parsing not ok";
					}
				}
			} else {
				response.Status(400).ResponsePhrase("Bad Request").Data("Invalid command");
			}
		} else {
			response.Status(400).ResponsePhrase("Bad Request").Data("Command format invalid");
		}
	} else {
		QString	filename;
		filename = VlcStreamerApp::Instance()->DocumentRoot() + _request.Url().path();
		QFile		file(filename);
		if(file.exists()) {
			if(file.open(QIODevice::ReadOnly)) {
				QByteArray	data = file.readAll();
				response.Data(data, filename.endsWith(".m3u8") ? "application/vnd.apple.mpegurl" : "application/octet-stream");
				file.close();
			} else {
				response.Status(404).ResponsePhrase("Not found").Data("File not found");
			}
		} else {
			response.Status(404).ResponsePhrase("Not found").Data("File not found");
		}
	}
	QByteArray ug;
	write(ug = response.ToByteArray());
	//qDebug() << ug;
	if(_request.Value("Connection") == "close") {
		disconnectFromHost();
	}
}


QVector<VlcStreamerConnection::_VideoInfo>	VlcStreamerConnection::_GetConvertedVideoInfo() const
{
	QVector<_VideoInfo>	rtn;
	QDir				dir(VlcStreamerApp::Instance()->DocumentRoot());

	QStringList	list = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) list.size(); ++i) {
		if(dir.cd(list.at(i))) {
			QFile	params(dir.path() + "/params.txt");
			if(params.open(QIODevice::ReadOnly)) {
				_VideoInfo	info;
				info.name = list.at(i).toAscii();
				info.params = params.readAll();
				info.status = "not processed";
				if(QFile::exists(dir.path() + "/complete.txt")) {
					info.status = "complete";
				} else if(QFile::exists(dir.path() + "/stream.m3u8")) {
					QFile	index(dir.path() + "/stream.m3u8");
					if(index.open(QIODevice::ReadOnly)) {
						QByteArray data = index.readAll();
						index.close();
						info.status = "segments-" + QByteArray::number(data.count("stream-"));
					}
				}
				params.close();
				rtn += info;
			}
			dir.cdUp();
		}
	}
	return rtn;
}


// ----------------------------------------------------------------


void VlcStreamerConnection::_Request::Clear()
{
	_complete = false;
	_method.clear();
	_url.clear();
	_version.clear();
	_headers.clear();
}


bool VlcStreamerConnection::_Request::AddLine(const QByteArray &line)
{
	if(_method.isEmpty()) {
		QList<QByteArray> list = line.split(' ');
		if(list.count() == 3 && list.at(2).contains("HTTP")) {
			_method = list.at(0);
			_url = QUrl::fromEncoded(list.at(1));
			_version = list.at(2);
			return true;
		}
	} else if(line == "\r\n") {
		_complete = true;
		return true;
	} else {
		int pos = line.indexOf(':');
		if(pos > 0) {
			_headers.insert(line.left(pos), line.mid(pos + 1).trimmed());
			return true;
		}
	}
	return false;
}


// ----------------------------------------------------------


VlcStreamerConnection::_Response::_Response()
{
	_version = "HTTP/1.1";
	_status = 200;
	_responsePhrase = "OK";
	Header("Server", "Gallandro Web Server");
	Header("Keep-Alive", "timeout=20, max=400");
	Header("Connection", "Keep-Alive");
}


VlcStreamerConnection::_Response &VlcStreamerConnection::_Response::Data(const QByteArray &data, const QByteArray &type)
{
	_data = data;
	Header("Content-Length", QByteArray::number(_data.size()));
	Header("Content-Type", type);
	return *this;
}


QByteArray VlcStreamerConnection::_Response::ToByteArray()
{
	QByteArray	rtn;

	Header("Date", QDateTime::currentDateTimeUtc().toString("dd MMM yyyy hh:mm:ss 'GMT'").toAscii());

	rtn = _version + ' ' + QByteArray::number(_status) + ' ' + _responsePhrase + "\r\n";

	for(unsigned i = 0; i != (unsigned) _headers.size(); ++i) {
		rtn += (QByteArray) _headers[i].first + ": " + _headers[i].second + "\r\n";
	}
	rtn += "\r\n";

	if(_data.isEmpty() == false) {
		rtn += _data;
	}

	return rtn;
}
