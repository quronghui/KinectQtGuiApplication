#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include "BodyDate.h"
#include "BodyDateByFlow.h"
#include "Human.h"
#include "MyTimer.h"
#include "Script.h"
#include "Unit.h"

using namespace std;
using namespace cv;

#pragma once
class MyMain : public MyTikSink
// 为了响应 MyTimer 的事件，继承了 MyTikSink
{
private:
	Mat				depthImage;			// 获取深度图
	Mat				colorImage;			// 获取有色图
	Mat				block;				// [!!!] 无作用，只是为了防止内存 bug，不能删
	MyTimer*		mt;					// 计时器
	Script*			script;				// 流程脚本
	BodyDate		bodyDate;			// 身体数据

	BodyDateByFlow	arm;				// 手臂数据流
	BodyDateByFlow	belly;				// 腹部数据流
	BodyDateByFlow	leg;				// 腿部数据流
	BodyDateByFlow	height;				// 高度数据流

	double			armShow;			// 手臂数据显示
	double			bellyShow;			// 腹部数据显示
	double			legShow;			// 腿部数据显示
	double			heightShow;			// 高度数据显示

	Rect rect[8];						// 人体所在矩形，因为有一个字节的 id，所以有 8 个
	Rect *r;							// 对上一个字段的指针，为了 id 为 0 不做使用导致的偏差
	int id;								// 当前聚焦的 id

	std::ostringstream oss;				// 输出字符串流
	string wrong;						// 记录错误信息

	CvPoint skeletonPoint[NUI_SKELETON_COUNT][NUI_SKELETON_POSITION_COUNT];
										// 骨骼点数组【id】【部位】
	HANDLE colorEvent;					// 主摄像头事件
	HANDLE depthEvent;					// 深度摄像头事件
	HANDLE skeletonEvent;				// 骨骼事件
	HANDLE colorStreamHandle;			// 主摄像头视频流
	HANDLE depthStreamHandle;			// 深度摄像头流
	HRESULT hr;							// 返回标志

	void TikSink(long time);			// 响应一个每秒发生的事件
	void getColorImage();				// 处理有色图
	void getDepthImage();				// 处理深度图
	// 处理骨骼点图 
	void getSkeletonImage();
	// 使用当前点扩充人体矩形
	inline void updateRectangle(Rect *r, Point p, int player)
	{
		if (r[player].tl().x > p.x || r[player].x == -1)
		{
			r[player].height += r[player].x - p.x;
			r[player].x = p.x;
		}
		if (r[player].tl().y > p.y || r[player].y == -1)
		{
			r[player].width += r[player].y - p.y;
			r[player].y = p.y;
		}
		if (r[player].br().x < p.x) r[player].width = p.x - r[player].x;
		if (r[player].br().y < p.y) r[player].height = p.y - r[player].y;
	}
public:
	MyMain(void);						// 构造函数
	int Frame();						// 处理一帧
	inline Mat GetColorImage() { return colorImage; }
	inline string GetTip()
	{
		oss.str("");
		oss << script->Run(mt->Tik());
		return oss.str();
	}
	inline string GetHight()
	{
		oss.str("");
		oss << "【参考】身高: " << Unit::ToCM(script->height) << "cm";
		return oss.str();
	}
	inline string GetArm()
	{
		oss.str("");
		oss << "【参考】手臂粗: " << Unit::ToCM(script->arm) << "cm";
		return oss.str();
	}
	inline string GetBelly()
	{
		oss.str("");
		oss << "【参考】腰围: " << Unit::ToCM(script->GetWaistline()) << "cm";
		return oss.str();
	}
	inline string GetLeg()
	{
		oss.str("");
		oss << "【参考】大腿粗: " << Unit::ToCM(script->leg) << "cm";
		return oss.str();
	}
	inline string GetWHtR()
	{
		oss.str("");
		oss << "腰高比: " << script->GetWHtR();
		return oss.str();
	}
	inline string GetHightTest()
	{
		oss.str("");
		mt->Tik();
		oss << Unit::ToCM(heightShow);
		return oss.str();
	}
	inline string GetWrong() { return wrong; }
	inline int Check(string type)
	{
		if (type == "hight")
		{
			if (Unit::ToCM(script->height) > 175) return 1;
			else if (Unit::ToCM(script->height) < 160) return -1;
			else return 0;
		}
		if (type == "arm")
		{
			if (Unit::ToCM(script->arm) > 24) return 1;
			else if (Unit::ToCM(script->arm) < 12) return -1;
			else return 0;
		}
		if (type == "belly")
		{
			if (Unit::ToCM(script->GetWaistline()) > 90) return 1;
			else if (Unit::ToCM(script->GetWaistline()) < 60) return -1;
			else return 0;
		}
		if (type == "leg")
		{
			if (Unit::ToCM(script->leg) > 30) return 1;
			else if (Unit::ToCM(script->leg) < 15) return -1;
			else return 0;
		}
		if (type == "WHtR")
		{
			if (script->GetWHtR() > 0.6) return 1;
			else if (script->GetWHtR() < 0.4) return -1;
			else return 0;
		}
	}
	inline bool IsDone() { return script->isDone(); }
	~MyMain();
};