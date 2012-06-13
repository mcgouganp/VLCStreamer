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
	virtual ~VlcStreamerConnection();

private slots:
	void	_RecvTimeout();
	void	_RecvData();
	void	_Disconnected();

private:
	class _Request {
	public:
		_Request()	{ Clear(); }
		void Clear();

		bool		AddLine(const QByteArray &line);
		bool		IsComplete() const				{ return _complete; }
		QByteArray	Method() const					{ return _method; }
		const QUrl	&Url() const					{ return _url; }
		QByteArray	Version() const					{ return _version; }
		QByteArray	Value(const QByteArray &name)	{ return _headers.value(name); }

	private:
		bool							_complete;
		QByteArray						_method;
		QUrl							_url;
		QByteArray						_version;
		QMap<QByteArray, QByteArray>	_headers;
	};

	class _Response {
	public:
		_Response();

		_Response	&Version(const QByteArray &version)							{ _version = version; return *this; }
		_Response	&Status(unsigned status)									{ _status = status; return *this; }
		_Response	&ResponsePhrase(const QByteArray &phrase)					{ _responsePhrase = phrase; return *this; }
		_Response	&Header(const QByteArray &name, const QByteArray &value)	{ _headers.append(qMakePair(name, value)); return *this; }
		_Response	&Data(const QByteArray &data, const QByteArray &type = "text/html");

		QByteArray	ToByteArray();

	private:
		QByteArray								_version;
		unsigned								_status;
		QByteArray								_responsePhrase;
		QVector<QPair<QByteArray, QByteArray> >	_headers;
		QByteArray								_data;
	};

	struct _VideoInfo {
		QByteArray	params;
		QByteArray	name;
		QByteArray	status;
	};

	static void		_DeleteDirectory(QDir &name);
	static QString	_ServerId();

	void	_Respond();
	
	QVector<_VideoInfo>	_GetConvertedVideoInfo() const;

	QTimer		_readTimer;
	_Request	_request;
};


#endif	// _VlcStreamerConnection_h