#include "DXUT.h"
#include "MyRnd.h"

MyRnd::MyRnd(void)
{
	Init(1);
	
}

MyRnd::~MyRnd(void)
{
}

void	MyRnd::Init(unsigned long seed)
{
	int i;

    x[0]=3; x[1]=seed;
    for(i=2;i<=31;i++) x[i] = 1103515245*x[i-1] + 12345;
    for(i=0;i<310;i++) Next();
}


long	MyRnd::Next(void)
{
    unsigned n=(unsigned)x[0];

    if (n==31) n=1; else n++;
    x[0]=n; x[n]+=(n>3?x[n-3]:x[n+31-3]);
    return(x[n]>>1)&0x7FFFFFFF;

}


double	MyRnd::NextD(void)
{
	return Next() * (1.0f/0x7FFFFFFF);
}

