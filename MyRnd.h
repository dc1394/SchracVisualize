#pragma once

class MyRnd
{
public:
	MyRnd(void);
	~MyRnd(void);
	void	Init(unsigned long seed);
	long	Next(void);
	double	NextD(void);
protected:
	unsigned long	x[32];

};
