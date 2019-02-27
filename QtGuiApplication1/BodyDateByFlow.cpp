#include "BodyDateByFlow.h"

BodyDateByFlow::BodyDateByFlow(void)
{	// 一秒有 30 帧，设置 35 保险 
	dates.reserve(35);
}

int BodyDateByFlow::Min()
{
	int count = dates.size();
	int min = 9999;
    for (int i = 0; i < count; i++)
    {	// 滤除负数和小于 9999 的数
        if(dates[i] > 0 && min > dates[i])
		{
			min = dates[i];
		}
    }
	return min == 9999 ? 0 : min;
}
int BodyDateByFlow::Max()
{
	int count = dates.size();
	int max = 0;
    for (int i = 0; i < count; i++)
    {	// 滤除负数和小于 9999 的数
        if(max < dates[i] && dates[i] < 9999)
		{
			max = dates[i];
		}
    }
	return max;
}
double BodyDateByFlow::Avg()
{
	int count = dates.size();
	int sum = 0;
	int real_size = 0;
    for (int i = 0; i < count; i++)
    {	// 滤除负数和小于 9999 的数
        if(dates[i] > 0 && dates[i] < 9999)
		{
			sum += dates[i];
			++real_size;
		}
    }
	if (real_size == 0) return 0;
	return sum * 1.0 / real_size;
}
void BodyDateByFlow::append(int date)
{
	dates.push_back(date);
}
void BodyDateByFlow::clear()
{
	dates.clear();
}
