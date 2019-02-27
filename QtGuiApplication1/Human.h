#pragma once
class Human
{
public:
	// 单位 px
	int heightPX;
	int armPX;
	int bellyPX;
	int legPX;
	static int heightCM;

	Human();
	Human(int height, int arm, int belly, int leg);
	double getArm();
	double getBelly();
	double getLeg();
	~Human();
};

