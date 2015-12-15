#ifndef VISTEK_UTIL__H_
#define VISTEK_UTIL__H_

#include <math.h>

//Round(1.234, 2) = 1.23
//Round(1.234, 0) = 1.0
//Round(123.4, -1) = 120.0
inline double v_Round(double dVal, short iPlaces)
{
	double dRetval;
	double dMod = 0.0000001;
	if (dVal < 0.0) dMod = -0.0000001;
	dRetval = dVal;
	dRetval += (5.0 / pow(10.0, iPlaces + 1.0));
	dRetval *= pow(10.0, iPlaces);
	dRetval = floor(dRetval + dMod);
	dRetval /= pow(10.0, iPlaces);
	return dRetval;
}



#endif//VISTEK_UTIL__h_