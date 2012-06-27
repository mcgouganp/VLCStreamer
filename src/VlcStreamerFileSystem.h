// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#ifndef _VlcStreamerFileSystem_h
#define _VlcStreamerFileSystem_h

#include <QObject>
#include <QDir>
#include <QHash>
#include <QVariant>

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
		QVariantMap	params;
		QString		status;
		QString		serverId;
	};

	QHash<QString, _ConvertedVideoInfo>	_cache;

	bool	_GetDirectoryInfo(const QDir &dir, _ConvertedVideoInfo &info);
};

#endif	// _VlcStreamerFileSystem_h
