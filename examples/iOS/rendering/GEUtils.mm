
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda PŽrez
//  iOS Game Engine
//
//  Utils
//
//  --- GEUtils.mm ---
//
//////////////////////////////////////////////////////////////////


#include "GEUtils.h"

//
//  GELine
//
GELine::GELine(double x0, double y0, double x1, double y1)
{
	Dx = x1 - x0;
	Dy = y1 - y0;

	vertical = false;
	horizontal = false;

	if(Dx == 0.0)
	{
		vertical = true;
		px0 = x0;
	}
	else if(Dy == 0.0)
	{
		horizontal = true;
		py0 = y0;
	}
	else
	{
		m = Dy / Dx;
		b = y0 - (m * x0);
	}
}

GELine::~GELine()
{
}

bool GELine::can_calculate_y()
{
	// it doesn't exist any solution if Dx = 0 (no solution or infinite)
	if(vertical)
		return false;
	else
		return true;
}

double GELine::y(double x)
{
	if(horizontal)
		return py0;

	return (m * x) + b;
}

bool GELine::can_calculate_x()
{
	// it doesn't exist any solution if Dy = 0 (no solution or infinite)
	if(horizontal)
		return false;
	else
		return true;
}

double GELine::x(double y)
{
	if(vertical)
		return px0;

	return (y - b) / m;
}
