#ifndef _VlcEncoder_h
#define _VlcEncoder_h

#include <QRunnable>
#include <QString>
#include <QProcess>

class VlcEncoder : public QRunnable {
public:
	VlcEncoder(const QString &path) : _scriptFilename(path)  {}

	void run();

protected:
	QString	_scriptFilename;
};
#endif	// _VlcEncoder_h
