#include <QCoreApplication>
#include "CamFiSocket.h"

#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	camfisocket* cfs = new camfisocket();
	cfs->takepicture();
	cfs->getfirminfo();
	cfs->takepicture();
	cfs->readpiclist(1, 5);
	cfs->takepicture();

	return a.exec();
}