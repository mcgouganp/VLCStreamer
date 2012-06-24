#ifndef _VlcStreamerConnection_h
#define _VlcStreamerConnection_h

#include <QDir>
#include <QTcpSocket>
#include <QTimer>
#include <QUrl>

class VlcStreamerConnection : public QTcpSocket {
	Q_OBJECT
public:
	VlcStreamerConnection(QObject *parent);

private slots:
	void	_RecvTimeout();
	void	_RecvData();
	void	_Disconnected();

private:
	class _Request {
	public:
		_Request()	{ Clear(); }
		void Clear();

		bool		AddLine(const QString &line);
		bool		IsComplete() const				{ return _complete; }
		QString		Method() const					{ return _method; }
		const QUrl	&Url() const					{ return _url; }
		QString		Version() const					{ return _version; }
		QString		Value(const QString &name)		{ return _headers.value(name); }

	private:
		bool					_complete;
		QString					_method;
		QUrl					_url;
		QString					_version;
		QMap<QString, QString>	_headers;
	};

	class _Response {
	public:
		_Response();

		_Response	&Version(const QString &version)					{ _version = version; return *this; }
		_Response	&Status(unsigned status)							{ _status = status; return *this; }
		_Response	&ResponsePhrase(const QString &phrase)				{ _responsePhrase = phrase; return *this; }
		_Response	&Header(const QString &name, const QString &value)	{ _headers.append(qMakePair(name, value)); return *this; }
		_Response	&Data(const QByteArray &data, const QString &type = "text/html");

		QByteArray	ToByteArray();

	private:
		QString								_version;
		unsigned							_status;
		QString								_responsePhrase;
		QVector<QPair<QString, QString> >	_headers;
		QByteArray							_data;
	};

	void	_Respond();
	
	QTimer		_readTimer;
	_Request	_request;
};


#endif	// _VlcStreamerConnection_h