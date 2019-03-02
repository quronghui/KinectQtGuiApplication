#include "QtGuiApplication1.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);		//This file is part of the QtWidgets module of the Qt Toolkit.
	QtGuiApplication1 w;
	w.show();
	return a.exec();
}
