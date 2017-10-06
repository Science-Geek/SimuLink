#include "SoftSimulink.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SoftSimulink w;
	w.show();
	return a.exec();
}
