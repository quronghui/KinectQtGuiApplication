#include "QtGuiApplication1.h"
#include <QFileDialog>


QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent)
{
	// qt 初始化
	ui.setupUi(this);
	// 设置一个 qt 计时器
	updateTime = new QTimer(this);
	// 连接重置按钮的点击事件和 reset() 函数
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(reset()));
	// 连接计时器的时间到事件和 updateFrame() 函数
	connect(updateTime, SIGNAL(timeout()), this, SLOT(updateFrame()));
	// 计时器开始计时 33 个毫秒
	updateTime->start(33);
	// 全屏
	this->setWindowState(Qt::WindowMaximized);
	// MyMain 类构造
	m = new MyMain();
}
void QtGuiApplication1::updateFrame()
{
	// 调用 update() 函数，里面包含 paintEvent(QPaintEvent*) 函数
	this->update();
}
void QtGuiApplication1::reset()
{
	delete(m);
	m = new MyMain();
	updateTime->start(33);
}
void QtGuiApplication1::paintEvent(QPaintEvent *event)
{
	// 当出现奇怪的情况，会返回 false 
	if (m->Frame())
	{
		// 当一个测量流程结束后
		if (m->IsDone())
		{
			// stackedWidget 组件从 page0 切换成 page1
			ui.stackedWidget->setCurrentIndex(1);
			// 停止计时器
			updateTime->stop();
			// 对后台传来的数据进行绑定
			ui.hight->setText(QString::fromLocal8Bit(m->GetHight().data()));
			ui.arm->setText(QString::fromLocal8Bit(m->GetArm().data()));
			ui.belly->setText(QString::fromLocal8Bit(m->GetBelly().data()));
			ui.leg->setText(QString::fromLocal8Bit(m->GetLeg().data()));
			ui.WHtR->setText(QString::fromLocal8Bit(m->GetWHtR().data()));
			// 将数据编入数组，使遍历与判断更容易
			string s[] = { "hight", "arm", "belly", "leg", "WHtR" };
			QLabel* q[] = {ui.hight, ui.arm, ui.belly, ui.leg, ui.WHtR};
			for (size_t i = 0; i < 5; i++)
			{
				int check = m->Check(s[i]);
				// 对 check 位进行分析，给出颜色变化，-1：蓝，1：红，0：黑（原色）
				if (check == 1)
					q[i]->setStyleSheet("color:red;");
				else if (check == -1)
					q[i]->setStyleSheet("color:blue;");
			}

		}
		else
		{
			// stackedWidget 组件从 page1 切换成 page0
			ui.stackedWidget->setCurrentIndex(0);
			cv::Mat outImage;
			// 图像格式转换 由 BGR 到 RGB，以适应 qt 的 QPainter
			cv::cvtColor(m->GetColorImage(), outImage, cv::COLOR_BGR2RGB);
			// 设置缩放大小，以高为基准，宽高比不变，[!!!] y 取 3 的倍数是为了程序不崩溃
			int y = this->height() - this->height() % 3;
			int x = y * 4 / 3;
			cv::resize(outImage, outImage, Size(x, y), 0, 0, 1);
			QImage disImage = QImage((const unsigned char*)(outImage.data), outImage.cols, outImage.rows, QImage::Format_RGB888);
			// 创建QPainter，将指针传入该设备
			QPainter draw(this);
			// 传入的 QPoint 使输出图像居中
			draw.drawImage(QPoint((this->width() - x)/2, (this->height() - y)/2), disImage);
			// 对后台传来的提示语进行绑定
			ui.tips->setText(QString::fromLocal8Bit(m->GetTip().data()));
		}
	}
	else
	{
		// 停止计时器
		updateTime->stop();
		// 对后台传来的错误警告进行绑定
		ui.tips->setText(QString::fromLocal8Bit(m->GetWrong().data()));
	}
}
