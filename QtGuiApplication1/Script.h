#include <iostream>
#include <string>
#include <sstream>
#include "BodyDateByFlow.h"
using namespace std;
#pragma once
class Script
{
public:

	enum class MODE : unsigned char { AOTO_RUN_ONCE, RUN_NECT, AUTO_RUN };
	enum class STATE : unsigned char { NONE, FACE, SIDE, DONE, RESTART };
	enum class WAIT : unsigned char { NONE, WAIT, WAIT_OVER };
	//bool isWait;
	//bool isWaitOver;



	double arm;
	double leg;
	double belly1;
	double height;
	double belly2;

	double GetWaistline();
	double GetWHtR();

	WAIT wait;

	Script(void);
	Script(char firstTestReadyTime, char secondTestReadyTime);
	string Run(long time);

	inline bool isFaceMeasure() { return state == STATE::FACE; }
	inline bool isSideMeasure() { return state == STATE::SIDE; }
	inline bool isDone() { return state == STATE::DONE; }

	inline void next() { if (mode == MODE::RUN_NECT && wait == WAIT::WAIT) wait = WAIT::WAIT_OVER; }
private:
	char firstTestReadyTime;
	char secondTestReadyTime;

	MODE mode;
	STATE state;

	long waitTime;



	static const string tips[6];

	inline string ToString(long num)
	{
		ostringstream os;
		os << num;
		string result;
		istringstream is(os.str());
		is >> result;
		return result;
	}

};



