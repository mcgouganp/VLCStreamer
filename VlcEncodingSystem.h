#ifndef _VlcEncodingSystem_h
#define _VlcEncodingSystem_h

#include <QObject>
#include <QString>

class VlcEncodingSystem : public QObject {
	Q_OBJECT
public:
	VlcEncodingSystem(QObject *parent = 0);

public slots:
	void	OnChange(const QString &paramFile);

protected:
	static QString	_ServerId();
};
#endif	// _VlcEncodingSystem_h
