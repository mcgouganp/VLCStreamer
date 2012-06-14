#ifndef _VlcStreamerFileSystem_h
#define _VlcStreamerFileSystem_h

#include <QObject>
#include <QDir>
#include <QHash>

class VlcStreamerFileSystem : public QObject {
	Q_OBJECT
public:
	VlcStreamerFileSystem(QObject *parent = 0);

	QString	ConvertedVideoInfo() const;
	QString	MakeServerId() const;
	void	DeleteMovie(const QString &serverId);

public slots:
	void	OnDirChange(const QString &arg);

protected:
	struct _ConvertedVideoInfo {
		QString	params;
		QString	status;
		QString	serverId;
	};

	QHash<QString, _ConvertedVideoInfo>	_cache;

	static void	_DeleteDirectory(QDir &dir);

	bool	_GetDirectoryInfo(const QDir &dir, _ConvertedVideoInfo &info);
};

#endif	// _VlcStreamerFileSystem_h
