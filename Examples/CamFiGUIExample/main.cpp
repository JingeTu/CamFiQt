#include "camfiqt_gui.h"
#include <qcoreapplication.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CAMFIQT_GUI w;
	w.show();
	return a.exec();
}
