#include "QtGuiApplication1.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])	// Qt GUI Programe. 项目名称为QtGuiApplication1，所以讲widget替换
{
	QApplication a(argc, argv);		//This file is part of the QtWidgets module of the Qt Toolkit.
	QtGuiApplication1 w;			// QtGuiApplication1.h里面定义之后的字段以及函数声明，以及槽的实现，都是在这里，Qt的逻辑功能设计主要是修改这个文件。
	w.show();
	return a.exec();
}
/*
*	1.函数第一次跳转，跳转到 Qt的逻辑设计函数：QtGuiApplication1
*/