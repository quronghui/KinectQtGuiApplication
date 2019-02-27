#include "MyTimer.h"

MyTimer::MyTimer()
{
	frames = 0;
	fps = 0;
}

MyTimer::MyTimer(MyTikSink* _mSink):mSink(_mSink)
{
	frames = 0;
	fps = 0;
	is_stop = true;
}
 
bool MyTimer::isStop()
{
	return is_stop;
} 
 
void MyTimer::Start()
{
	start_time = time(0);
	now_to_start = 0;
	is_stop = false;
}
void MyTimer::Stop() 
{
	is_stop = true;
}
bool MyTimer::isComing(int num, string unit)
{
	if(unit.compare("s") == 0 || unit.compare("S") == 0 || unit.compare("") == 0)
		return (time(0) - start_time) > num;
	else if (unit.compare("m") == 0 || unit.compare("M") == 0 || unit.compare("min") == 0 || unit.compare("MIN") == 0 || unit.compare("Min") == 0)
		return (time(0) - start_time)/60 > num;
	else if (unit.compare("h") == 0 || unit.compare("H") == 0 || unit.compare("hour") == 0 || unit.compare("HOUR") == 0 || unit.compare("Hour") == 0)
		return (time(0) - start_time)/60/60 > num;
}
long MyTimer::Tik()
{
	long new_time = time(0) - start_time;
	++frames;
	if(now_to_start < new_time)
	{
		now_to_start = new_time;
		mSink->TikSink(new_time);

		fps = frames;
		frames = 0;
	}
	return new_time;
}
int MyTimer::GetFPS()
{
	return fps;
}

