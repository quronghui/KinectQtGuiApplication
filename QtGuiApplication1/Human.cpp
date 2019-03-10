﻿#include "Human.h"

int Human::heightCM = 160;

Human::Human(void)
{
}

// 还没有找到在哪里调用Hunman函数
// 函数的功能便是接收人体的数据
Human::Human(int height, int arm, int belly, int leg)
{
	heightPX = height;
	armPX = arm;
	bellyPX = belly;
	legPX = leg;
}
double Human::getArm()
{	
	if (heightPX == 0) return -2; 
	int armCM = heightCM * armPX * 1.0 / heightPX;
	return armCM <= 0 || armCM > 9999 ? -1 : armCM;
}
double Human::getBelly()
{
	if (heightPX == 0) return -2;
	int bellyCM = heightCM * bellyPX * 1.0 / heightPX;
	return bellyCM <= 0 || bellyCM > 9999 ? -1 : bellyCM;
}
double Human::getLeg()
{
	if (heightPX == 0) return -2;
	int legCM = heightCM * legPX * 1.0 / heightPX;
	return legCM <= 0 || legCM > 9999 ? -1 : legCM;
}

Human::~Human(void)
{
}
