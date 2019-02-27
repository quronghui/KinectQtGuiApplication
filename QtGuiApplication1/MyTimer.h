#include <iostream>
#include <ctime>
using namespace std;

#pragma once


class MyTikSink  
{  
public:  
    virtual void TikSink(long time) = 0;  
};

class MyTimer
{
public:
	MyTimer();
	MyTimer(MyTikSink* _mSink);
	bool isStop();
	void Start();
	// or restart it 
	void Stop();
	inline long getStartTime() { return start_time; }
	bool isComing(int num, string unit);
	long Tik();
	int GetFPS();

private:
	bool is_stop;

	short frames;
	short fps;

	long start_time;
	long now_to_start;

	MyTikSink* mSink;

};
