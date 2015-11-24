/**
*************************************************************************
*
* @file rec_containers.cpp
*
* 
*
************************************************************************/

#include "rec_containers.h"


namespace rec {


	std::ostream& operator<<(std::ostream& os, const rec::seAndFrust& c)
	{
		os << c.fittedEllipse.xc << ',' << c.fittedEllipse.yc;

		//os << "contour size: " << c.contour.size() << "  offset: " <<  c.offSetX << ',' << c.offSetY;
		return os;
	}




	std::ostream& operator<<(std::ostream& os, const rec::seAndPyramid& c)
	{
		os << c.fittedEllipse.xc << ',' << c.fittedEllipse.yc;

		//os << "contour size: " << c.contour.size() << "  offset: " <<  c.offSetX << ',' << c.offSetY;
		return os;
	}



}

