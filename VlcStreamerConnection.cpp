#include "VlcStreamerConnection.h"
#include <QDateTime>
#include <QFile>
#include <qjson/parser.h>
#include <qjson/serializer.h>
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
	QString	data;

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


void VlcStreamerConnection::_Respond()
{
	_Response	response;

	qDebug() << _request.Url().path();
	if(_request.Url().path() == "/secure") {
		QHash<QString, QString>	queryItems;

		QList<QPair<QString, QString> >		items;
		items = _request.Url().queryItems();
		qDebug() << "Items:" << items.size() << items;
		for(unsigned i = 0; i != (unsigned) items.size(); ++i) {
			queryItems.insert(items.at(i).first, items.at(i).second);
		}

		QByteArray	data;
		if(queryItems.contains("command")) {
			QString	command;
			command = queryItems.value("command");
			if(command == "movies") {
				response.Data(VlcStreamerApp::Instance()->Movies().ConvertedVideoInfo().toAscii());
			} else if(command == "create") {
				QDir		dir(VlcStreamerApp::Instance()->DocumentRoot());
				QFileInfo	src(queryItems.value("file"));
				QFile		params(dir.path() + "/_Queue/" + src.fileName() + ".params.txt");
				if(params.open(QIODevice::WriteOnly)) {
					QVariantMap	paramInfo;
					QHashIterator<QString, QString>	iter(queryItems);
					while(iter.hasNext()) {
						iter.next();
						QString	name, value;
						name = iter.key();
						value = iter.value();
						paramInfo.insert(name.replace("\"", "\\\""), value.replace("\"", "\\\""));
					}
					paramInfo.insert("serverid", VlcStreamerApp::Instance()->Movies().MakeServerId());
					QJson::Serializer	serializer;
					data = serializer.serialize(paramInfo);

					params.write(data);
					params.close();
				} else {
					qDebug() << "Couldn't open params file for writing" << params.fileName();
				}
				response.Data("Encoding");

			} else if(command == "getpath" || command == "browse") {
				QString	path;
				if(command == "getpath") {
					if(queryItems.value("dir") == "...drives...") {
						path = VlcStreamerApp::Instance()->DrivesDir();
					} else {
						path = VlcStreamerApp::Instance()->HomeDir();
					}
				} else {
					path = queryItems.value("dir");
				}
				QDir	dir(path);
				QFileInfoList	list = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot);

				QVariantList	files;
				for(unsigned i = 0; i != (unsigned) list.size(); ++i) {
					QVariantMap	fileInfo;
					fileInfo.insert("name", list.at(i).fileName());
					fileInfo.insert("path", list.at(i).filePath());
					fileInfo.insert("type", list.at(i).isDir() ? "dir" : "file");
					files.append(fileInfo);
				}
				QVariantMap		info;
				info.insert("files", files);
				info.insert("root", path);
				QJson::Serializer	serializer;
				data = serializer.serialize(info);
				response.Data(data);
			} else if(command == "delete") {
				VlcStreamerApp::Instance()->Movies().DeleteMovie(queryItems.value("id"));

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
				response.Data(file.readAll(), filename.endsWith(".m3u8") ? "application/vnd.apple.mpegurl" : "application/octet-stream");
				file.close();
			} else {
				response.Status(404).ResponsePhrase("Not found").Data("File not found");
			}
		} else {
			response.Status(404).ResponsePhrase("Not found").Data("File not found");
		}
	}
	write(response.ToByteArray());
	//qDebug() << response.ToByteArray();
	if(_request.Value("Connection") == "close") {
		disconnectFromHost();
	}
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


bool VlcStreamerConnection::_Request::AddLine(const QString &line)
{
	if(_method.isEmpty()) {
		QList<QString> list = line.split(' ');
		if(list.count() == 3 && list.at(2).contains("HTTP")) {
			_method = list.at(0);
			_url = QUrl::fromEncoded(list.at(1).toAscii());
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


VlcStreamerConnection::_Response &VlcStreamerConnection::_Response::Data(const QByteArray &data, const QString &type)
{
	_data = data;
	Header("Content-Length", QString::number(_data.size()));
	Header("Content-Type", type);
	return *this;
}


QByteArray VlcStreamerConnection::_Response::ToByteArray()
{
	QByteArray	rtn;
	QString		temp;

	Header("Date", QDateTime::currentDateTimeUtc().toString("dd MMM yyyy hh:mm:ss 'GMT'"));

	temp = _version + ' ' + QString::number(_status) + ' ' + _responsePhrase + "\r\n";

	for(unsigned i = 0; i != (unsigned) _headers.size(); ++i) {
		temp += (QString) _headers[i].first + ": " + _headers[i].second + "\r\n";
	}
	temp += "\r\n";

	rtn = temp.toAscii();
	if(_data.isEmpty() == false) {
		rtn += _data;
	}

	return rtn;
}
