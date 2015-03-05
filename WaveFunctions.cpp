#include "DXUT.h"
#include	"WaveFunctions.h"


//Šm—¦–§“x‚ğ—^‚¦‚éŠÖ”

//”¼Œa‚P‚Ì‹…‚Ì“à•”‚Í‚PAŠO•”‚Í‚O‚É‚È‚éŠÖ”

float	prob(float x, float y, float z)
{
	float r = sqrt( x*x + y*y + z*z);
	if( r< 1.0f)
	{
		return 1.0f;
	} else {
		return 0.0f;
	}
}

//…‘fŒ´q1py‹O“¹ Z=1 a_0 = 1‚Æ‚·‚é


float	prob2s(float x, float y, float z)
{
const	float a0 = 0.5f;

	float r = sqrt( x*x + y*y + z*z);
	float r0 = r / a0;
	float p = 0.5f * sqrt(0.5f) / (a0 * sqrt(a0)) * (2.0f-r0) * exp( - r0 *0.5f ); //“®Œa•ûŒüŠÖ”
	p *= 0.5f * sqrt(  M_1_PI) ;

	return p;
}

float	prob2py(float x, float y, float z)
{
const	float a0 = 1.0f;
	float r = sqrt( x*x + y*y + z*z);
	float r0 = r / a0;
	float p = 0.5f / sqrt(6.0f) / (a0 * sqrt(a0)) * r0 * exp( - r0 *0.5f ); //“®Œa•ûŒüŠÖ”
	p *= 0.5f * sqrt( 3.0f * M_1_PI) * y / r;

	return p;
}

float	prob2sp(float x, float y, float z)
{
const	float a0 = 0.5f;
const	float Z = 6.0f; //’Y‘f
const	float A = Z / a0;
	float r = sqrt( x*x + y*y + z*z);
	float r0 = r * A;
	float p1 = 0.5f * sqrt( 0.5f) * (A * sqrt(A)) * (2.0f-r0) * exp( - r0 *0.5f ); //“®Œa•ûŒüŠÖ”
	p1 *= 0.5f * sqrt(  M_1_PI) ; //p1 = s

	float p2 = 0.5f / sqrt(6.0f) * (A * sqrt(A)) * r0 * exp( - r0 *0.5f ); //“®Œa•ûŒüŠÖ”
	p2 *= 0.5f * sqrt( 3.0f * M_1_PI) * y / r; //p2 = p
	
	float p = sqrt(0.5f) * (p1 + p2);

	return p;
}

float	prob3d(float x, float y, float z)
{
const float a0 = 0.2f;

	float r = sqrt( x*x + y*y + z*z);
	float r0 = r / a0;
//	float p = 4.0f / 81.0f / sqrt(30.0f) / (a0 * sqrt(a0)) * (27.0f - 18.0f *r0 + 2.0f *a0*a0  ) * exp( - r0 / 3.0f ); //“®Œa•ûŒüŠÖ”
	float p = 4.0f / 81.0f / sqrt(30.0f) / (a0 * sqrt(a0)) * r0*r0 * exp( - r0 / 3.0f ); //“®Œa•ûŒüŠÖ”
	p *= 0.25f * sqrt( 5.0f * M_1_PI) * (2.0f*z*z -x*x -y*y) / (r*r);

	return p;
}

float	prob3dzx(float x, float y, float z)
{
const float a0 = 0.2f;

	float r = sqrt( x*x + y*y + z*z);
	float r0 = r / a0;
//	float p = 4.0f / 81.0f / sqrt(30.0f) / (a0 * sqrt(a0)) * (27.0f - 18.0f *r0 + 2.0f *a0*a0  ) * exp( - r0 / 3.0f ); //“®Œa•ûŒüŠÖ”
	float p = 4.0f / 81.0f / sqrt(30.0f) / (a0 * sqrt(a0)) * r0*r0 * exp( - r0 / 3.0f ); //“®Œa•ûŒüŠÖ”
	p *= 0.5f * sqrt( 15.0f * M_1_PI) * z*x / (r*r);

	return p;
}

