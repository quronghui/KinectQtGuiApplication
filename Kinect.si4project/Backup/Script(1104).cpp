#include "Script.h"

const string Script::tips[] = {
		"请平举双臂，双腿并拢，面对摄像头",
		"正在测量",
		"请向左转90度，侧对摄像头",
		"正在测量",
		"本次测量已结束",
		"正在初始化"
};

Script::Script(void)
{
	mode = MODE::RUN_NECT;
	state = STATE::FACE;
	wait = WAIT::WAIT;

	//isWait = false;
	//isWaitOver = false;

	waitTime = -2;

	height = 0;
	arm = 0;
	belly1 = 0;
	belly2 = 0;
	leg = 0;
}

Script::Script(char firstTestReadyTime, char secondTestReadyTime)
{
	mode = MODE::AOTO_RUN_ONCE;
	state = STATE::NONE;

	//isWait = false;
	//isWaitOver = false;

	this->firstTestReadyTime = firstTestReadyTime;
	this->secondTestReadyTime = secondTestReadyTime;


	height = 0;
	arm = 0;
	belly1 = 0;
	belly2 = 0;
	leg = 0;
}


string Script::Run(long time)
{
	if (mode == MODE::AOTO_RUN_ONCE || mode == MODE::AUTO_RUN)
	{
		if (time < firstTestReadyTime) return tips[0] + "（" + ToString(firstTestReadyTime - time) + "）";
		if (time == firstTestReadyTime) { state = STATE::FACE; return tips[1]; }
		if (time < firstTestReadyTime + secondTestReadyTime + 1) { state = STATE::NONE; return tips[2] + "（" + ToString(firstTestReadyTime + 1 + secondTestReadyTime - time) + "）"; }
		if (time == firstTestReadyTime + secondTestReadyTime + 1) { state = STATE::SIDE; return tips[3]; }
		if (time > firstTestReadyTime + secondTestReadyTime + 5 && mode == MODE::AUTO_RUN) { state = STATE::RESTART; return ""; }
		state = STATE::DONE;
		return tips[4];
	}
	else
	{
		if (waitTime != -2)
		{
			if (waitTime == time - 2)
			{
				state = STATE(int(state) + 1);
				cout << "state change" << endl;
				waitTime = -2;
				wait = WAIT::WAIT;
				return "";
			}
			else
			{
				cout << "<<<<<<<" << endl;
				if (state == STATE::DONE) return tips[5];
				return tips[1];
			}
		}
		else
		{
			if (state == STATE::FACE && wait == WAIT::WAIT) return tips[0];
			if (state == STATE::FACE && wait == WAIT::WAIT_OVER) { waitTime = time; wait = WAIT::NONE; return tips[1]; }
			if (state == STATE::SIDE && wait == WAIT::WAIT) return tips[2];
			if (state == STATE::SIDE && wait == WAIT::WAIT_OVER) { waitTime = time; wait = WAIT::NONE; return tips[3]; }
			if (state == STATE::DONE && wait == WAIT::WAIT) return tips[4];
			if (state == STATE::DONE && wait == WAIT::WAIT_OVER) { waitTime = time; wait = WAIT::NONE; return tips[5]; }
			state = STATE::FACE;
			wait = WAIT::WAIT;
			return tips[5];
		}
	}
}
double Script::GetWaistline()
{
	double belly_long = 0;
	double belly_short = 0;
	if (belly1 > belly2)
	{
		belly_long = belly1;
		belly_short = belly2;
	}
	else
	{
		belly_long = belly2;
		belly_short = belly1;
	}
	return (3.14515926 * belly_short + 2 * (belly_long - belly_short)) * 0.8;
}
double Script::GetWHtR()
{
	return GetWaistline() / height;
}

