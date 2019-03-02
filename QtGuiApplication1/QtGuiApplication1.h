#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QStackedlayout>
#include <opencv2/opencv.hpp>
#include "MyMain.h"
#include "ui_QtGuiApplication1.h"

// QtGuiApplication1(工程的名字Widget)类的头文件，定义一些字段和函数声明，
//包括最重要的slots（槽）函数的声明，以及界面ui句柄，以便通过“ui.***”的方式访问到界面的各个控件，
// 比如访问界面的Label控件里的文字可以这样：ui.label->text();就是字面意思，很容易理解。
class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);
	void paintEvent(QPaintEvent *event);

public slots:							// 2.两个槽函数，响应事件
	void updateFrame();					// 连接计时器的时间到事件和 updateFrame() 函数
	void reset();						// 对应QtGuiApplication.cpp 里面的reset()槽函数 

private:
	QTimer* updateTime;					//1.设置Qt定时器，
	MyMain* m;							// 3. Mymain 函数的定义
	Ui::QtGuiApplication1Class ui;
};
