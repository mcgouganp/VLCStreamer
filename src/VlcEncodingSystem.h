// Copyright (C) 2012 Paul McGougan
// 
// Distributed under the GNU General Public License 2.0. (See acommpanying file
// LICENSE or copy at http://www.gnu.org/licenses/).

#ifndef _VlcEncodingSystem_h
#define _VlcEncodingSystem_h

#include <QObject>
#include <QString>

class VlcEncodingSystem : public QObject {
	Q_OBJECT
public:
	VlcEncodingSystem(QObject *parent = 0);

public slots:
	void	OnChange(const QString &arg);

protected:
	void	_DoEncode(const QString &paramFile);
};
#endif	// _VlcEncodingSystem_h
