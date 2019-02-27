#pragma once
class Unit
{
private:
	static const double k;

public:
	

	Unit(void);
	inline static double ToCM(int px)
	{
		return px * k;
	}
};

