#include "BodyDate.h"

BodyDate::BodyDate(){}
BodyDate::BodyDate(CvPoint skeletonPoint[][NUI_SKELETON_POSITION_COUNT])
{	// 遍历所有 id
	for (int i = 0; i < 8; i++)
	{	// 使用手肘关节，臀部关节，膝盖与胯部关节中间点作为基准点
		body[i][ARM_LE] = cv::Rect(skeletonPoint[i][NUI_SKELETON_POSITION_ELBOW_LEFT], skeletonPoint[i][NUI_SKELETON_POSITION_ELBOW_LEFT]);
		body[i][ARM_RI] = cv::Rect(skeletonPoint[i][NUI_SKELETON_POSITION_ELBOW_RIGHT], skeletonPoint[i][NUI_SKELETON_POSITION_ELBOW_RIGHT]);
		body[i][BELLY_LE] = cv::Rect(skeletonPoint[i][NUI_SKELETON_POSITION_HIP_CENTER], skeletonPoint[i][NUI_SKELETON_POSITION_HIP_CENTER]);
		body[i][BELLY_RI] = cv::Rect(skeletonPoint[i][NUI_SKELETON_POSITION_HIP_CENTER], skeletonPoint[i][NUI_SKELETON_POSITION_HIP_CENTER]);
		cv::Point middle = (cv::Point(skeletonPoint[i][NUI_SKELETON_POSITION_HIP_LEFT]) + cv::Point(skeletonPoint[i][NUI_SKELETON_POSITION_KNEE_LEFT]))*0.5;
		body[i][LEG_LE] = cv::Rect(middle.x, middle.y, 0, 0);
		middle = (cv::Point(skeletonPoint[i][NUI_SKELETON_POSITION_HIP_RIGHT]) + cv::Point(skeletonPoint[i][NUI_SKELETON_POSITION_KNEE_RIGHT]))*0.5;
		body[i][LEG_RI] = cv::Rect(middle.x, middle.y, 0, 0);
	}
}

cv::Rect BodyDate::getLine(cv::Point checkPoint, cv::Rect output, bool isHight)
{	// 如果画竖线，与基准点 x 轴相同
	if (isHight && checkPoint.x == output.x)
	{
		if (output.tl().y > checkPoint.y)
		{
			output.height += output.y - checkPoint.y;
			output.y = checkPoint.y;
		}
		if (output.br().y < checkPoint.y) output.height = checkPoint.y - output.y;
	}// 如果画水平线，与基准点 y 轴相同
	else if (isHight == false && checkPoint.y == output.y)
	{
		if (output.tl().x > checkPoint.x)
		{
			output.width += output.x - checkPoint.x;
			output.x = checkPoint.x;
		}
		if (output.br().x < checkPoint.x) output.width = checkPoint.x - output.x;
	}
	return output;
}



void BodyDate::updateBody(cv::Point p, int id)
{
	if (body[id][BodyDate::ARM_LE].x != 0 && body[id][BodyDate::ARM_LE].x != 0)
	{
		getArmHight(p, body[id]);
		getLegWidth(p, body[id]);
		getBellyWidth(p, body[id]);
	}
}
int BodyDate::getDate(std::string s, int id)
{
	if (s == "arm")
		return (body[id][BodyDate::ARM_LE].height + body[id][BodyDate::ARM_RI].height)/2;
	if (s == "leg")
		return (body[id][BodyDate::LEG_LE].width + body[id][BodyDate::LEG_RI].width)/2;
	if (s == "belly")
		return (body[id][BodyDate::BELLY_LE].width+ body[id][BodyDate::BELLY_RI].width)/2;
	return 0;
}

BodyDate::~BodyDate(void)
{
}
