#include <windows.h>
#include <iostream> 
#include <NuiApi.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "opencv2/imgproc/imgproc.hpp"

#pragma once
class BodyDate
{
	public:
		enum BODY_BLOCK{ARM_LE, ARM_RI, LEG_LE, LEG_RI, BELLY_LE, BELLY_RI, bodyLen};
		cv::Rect body[8][bodyLen];

		BodyDate();
		BodyDate(CvPoint skeletonPoint[][NUI_SKELETON_POSITION_COUNT]);
		void updateBody(cv::Point p, int id);
		int getDate(std::string s, int id);
		~BodyDate(void);
	private:
		static cv::Rect getLine(cv::Point checkPoint, cv::Rect output, bool isHight);
		inline void getArmHight(cv::Point checkPoint, cv::Rect body[])
		{
			body[BodyDate::ARM_LE] = BodyDate::getLine(checkPoint, body[BodyDate::ARM_LE], true);
			body[BodyDate::ARM_RI] = BodyDate::getLine(checkPoint, body[BodyDate::ARM_RI], true);
		}
		inline void getLegWidth(cv::Point checkPoint, cv::Rect body[])
		{
			body[BodyDate::LEG_LE] = BodyDate::getLine(checkPoint, body[BodyDate::LEG_LE], false);
			body[BodyDate::LEG_RI] = BodyDate::getLine(checkPoint, body[BodyDate::LEG_RI], false);
		}
		inline void getBellyWidth(cv::Point checkPoint, cv::Rect body[])
		{
			body[BodyDate::BELLY_LE] = BodyDate::getLine(checkPoint, body[BodyDate::BELLY_LE], false);
			body[BodyDate::BELLY_RI] = BodyDate::getLine(checkPoint, body[BodyDate::BELLY_RI], false);
		}
};

