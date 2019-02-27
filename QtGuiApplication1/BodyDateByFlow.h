#include <vector>
using namespace std;
#pragma once
class BodyDateByFlow
{
private:
	vector <int> dates;
public:
	BodyDateByFlow(void);
	int Min();
	int Max();
	double Avg();
	void append(int date);
	void clear();
};

