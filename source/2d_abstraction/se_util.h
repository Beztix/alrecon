#ifndef SE_UTIL_H
#define SE_UTIL_H




#include <opencv2/core/core.hpp>
#include <viral_core/geo_2d.hpp>


namespace se {



	double evaluateFitImplicid(std::vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon);

	double evaluateFitEuclidean(std::vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon);

	bool isFitConservative(std::vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon);


	std::vector<viral_core::vector2f> getBoundingboxOfSuperellipse(double xc, double yc, double a, double b, double epsilon, double theta);



	// a struct representing a single superellipse

	struct superellipse {
		float xc;			//x-coordinate of the center
		float yc;			//y-coordinate of the center
		float theta;		//angle
		float a;			//length of the longer axis
		float b;			//length of the shorter axis
		float epsilon;		//form factor
		float quality;		//quality of the fit achieved with this superellipse


		superellipse() {
			xc = 0;
			yc = 0;
			theta = 0;
			a = 0;
			b = 0;
			epsilon = 0;
			quality = -1;
		}



		superellipse(float mxc, float myc, float mtheta, float ma, float mb, float mepsilon, float mquality) {
			xc = mxc;
			yc = myc;
			theta = mtheta;
			a = ma;
			b = mb;
			epsilon = mepsilon;
			quality = mquality;
		}
	};






	//a struct representing a contour, including its width, height and offset and additionally a superellipse if it was already fitted to the contour

	struct contourAndSe {
		std::vector<cv::Point> contour;	//the contour
		int width;
		int height;
		int offSetX;
		int offSetY;
		bool alreadyFitted;
		se::superellipse fittedEllipse;	//the superellipse fitted to the contour


		//constructor creating an empty contourAndSe
		contourAndSe() :
			width(0), height(0), offSetX(0), offSetY(0), alreadyFitted(false), fittedEllipse()
		{
		}


		//constructor creating a contourAndSe without a fitted ellipse
		contourAndSe(std::vector<cv::Point> mcontour, int mwidth, int mheight, int moffSetX, int moffSetY) :
			contour(mcontour), width(mwidth), height(mheight), offSetX(moffSetX), offSetY(moffSetY), alreadyFitted(false), fittedEllipse()
		{
		}


		//constructor creating a contourAndSe with a fitted ellipse
		contourAndSe(std::vector<cv::Point> mcontour, int mwidth, int mheight, int moffSetX, int moffSetY, se::superellipse mfittedEllipse) :
			contour(mcontour), width(mwidth), height(mheight), offSetX(moffSetX), offSetY(moffSetY), alreadyFitted(true), fittedEllipse(mfittedEllipse)
		{
		}


		void setEllipse(se::superellipse mfittedEllipse) {
			alreadyFitted = true;
			fittedEllipse = mfittedEllipse;
		}
	};


	std::ostream& operator<<(std::ostream& os, const se::contourAndSe& c);


}








#endif /* SE_UTIL_H*/
