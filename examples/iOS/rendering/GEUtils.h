
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Utils
//
//  --- GEUtils.h ---
//
//////////////////////////////////////////////////////////////////


#ifndef _GEUTILS_H_
#define _GEUTILS_H_

#include <stdlib.h>
#include <cmath>

//
//  GELine
//
class GELine
{
private:
	double px0;
	double py0;

	double Dx;
	double Dy;

	double m;
	double b;

	bool vertical;
	bool horizontal;

public:
	GELine(double x0, double y0, double x1, double y1);
	~GELine();

	bool can_calculate_y();
	double y(double x);

	bool can_calculate_x();
	double x(double y);
};

#endif
