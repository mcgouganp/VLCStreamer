#include "Utils.h"


void DeleteDirectory(QDir &dir)
{
	QFileInfoList	files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) files.size(); ++i) {
		dir.remove(files.at(i).fileName());
	}
	QFileInfoList	subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(unsigned i = 0; i != (unsigned) subdirs.size(); ++i) {
		dir.cd(subdirs.at(i).fileName());
		DeleteDirectory(dir);
	}
	QString current = dir.dirName();
	dir.cdUp();
	dir.rmdir(current);
}
