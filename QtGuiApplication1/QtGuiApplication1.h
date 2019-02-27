#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QTimer>
#include <QStackedlayout>
#include <opencv2/opencv.hpp>
#include "MyMain.h"
#include "ui_QtGuiApplication1.h"

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);
	void paintEvent(QPaintEvent *event);

public slots:
	void updateFrame();
	void reset();

private:
	MyMain* m;
	Ui::QtGuiApplication1Class ui;
	QTimer* updateTime;
};
