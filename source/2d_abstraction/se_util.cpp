#include "se_util.h"


#include <vector>
#include <opencv2/core/core.hpp>



using namespace std;


namespace se {

	



	/// evaluates the quality of a superellipsefit using the implicid superellipse equation, lower fitValue is better
	/**
	* This method uses the contour and the parameters of the superellipse fitted to that contour to calculate a quality of the fit (fitValue).
	* The fitValue is calculated as the sum of the values received by putting the points of the contour in the implicid superellipse equation.
	*/

	double evaluateFitImplicid(vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon) {
		double cosine2 = cos(-theta);
		double sine2 = sin(-theta);
		double tx, ty;
		double x, y, x2, y2, qualityValue;
		double totalValue = 0;

		double exponent = 2.0 / epsilon;

		//for each point of the contour:
		for (int i = 0; i < contour.size(); i++){
			x = contour.at(i).x;
			y = contour.at(i).y;

			//transform coordinates in the coordinatesystem of the superellipse
			tx = x - xc;
			ty = y - yc;
			x2 = (tx * cosine2 - ty * sine2);
			y2 = (tx * sine2 + ty * cosine2);

			//calculate the value of the point in the superellipse equation
			double firstTerm = (fabs(x2) / a);
			double secondTerm = (fabs(y2) / b);
			double equationValue = pow(firstTerm, exponent) + pow(secondTerm, exponent) - 1.0;

			//if the value is < 0, so the point is on the inside of the superellipse:
			if (equationValue < 0) {
				//the value is close to -1, to receive a useful quality value add 1 and invert:
				double temp = 1.0001 + equationValue;	//1.0001 instead of 1 so that the inverted value doesnt get incredibly big
				qualityValue = 1 / temp;
			}
			//if the value is > 0, so the point is on the outside of the superellipse:
			else {
				//the value is already a useful quality value
				qualityValue = equationValue;
			}

			totalValue += qualityValue;
		}


		//cout << "Total Value: " << totalValue << endl;
		return totalValue;

	}











	/// evaluates the quality of a superellipsefit using the euclidean distance between contour points and superellipses, lower fitValue is better
	/**
	* This method uses the contour and the parameters of the superellipse fitted to that contour to calculate a quality of the fit (fitValue).
	* The fitValue is calculated as the sum of the euclidean distances between the points of the contour and the superellipse along a line through the
	* contour point and the center of the superellipse.
	* For larger contours, not every contour point is evaluated, but instead an appropriate stepsize is chosen.
	*/

	double evaluateFitEuclidean(vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon) {
		double cosine2 = cos(-theta);
		double sine2 = sin(-theta);
		double x, y, xTrans, yTrans, xRel, yRel;
		double totalOutDist = 0;
		double totalInDist = 0;

		//calculations not depending on x, y
		double exponent = 2.0 / epsilon;
		double exponentInv = epsilon / 2.0;
		double termOne = 1 / pow(a, exponent);

		//calculate a stepsize
		int stepsize;
		if (contour.size() < 20) {
			stepsize = 1;
		}
		else if (contour.size() > 400) {
			stepsize = 20;
		}
		else {
			double stepsizeD = contour.size() / 20;
			stepsize = (int)stepsizeD;
		}

		//for each point of the contour:
		for (int i = 0; i < contour.size(); i += stepsize){
			x = contour.at(i).x;
			y = contour.at(i).y;

			//transform coordinates to the coordinatesystem of the superellipse
			xTrans = x - xc;
			yTrans = y - yc;
			xRel = (xTrans * cosine2 - yTrans * sine2);
			yRel = (xTrans * sine2 + yTrans * cosine2);

			//evaluate only in first quadrant
			xRel = fabs(xRel);
			yRel = fabs(yRel);


			//calculate the euclidean distance between the point and the point (xs, ys) on the superellipse
			double divOne = termOne + fabs(pow((yRel / (xRel*b)), exponent));
			double xs = pow((fabs(1 / (divOne))), exponentInv);
			double ys = xs * (yRel / xRel);
			double dist = (xRel - xs) * (xRel - xs) + (yRel - ys) * (yRel - ys);

			//special numerical case: xRel near 0, calculation might result in dist INF
			if (xRel < 0.00001) {
				dist = fabs(b - yRel);
				if (yRel < b) { //point on the inside of the superellipse
					xs = xRel + 1;
				}
				else { //point on the outside of the superellipse
					xs = xRel - 1;
				}
			}

			if (isinf(dist)) {
				double temp = 5.0;
			}


			//test if point is inside or outside of the superellipse (everything evaluated in first quadrant!)
			if (xs < xRel) {
				totalOutDist += dist;
			}
			else {
				totalInDist += dist;
			}

		}

		//calculate the stepsize back into the value
		totalInDist *= stepsize;
		totalOutDist *= stepsize;

		return totalInDist + totalOutDist;

	}





	/// checks if the fit of a superellipse to a contour is conservative, i.e. every point of the contour lies on or inside of the superellipse
	/**
	* This method uses the contour and the parameters of the superellipse fitted to that contour to calculate if the fit is conservative.
	* This is achieved by checking for each point of the contour, if the point lies inside of or on the superellipse.
	*/

	bool isFitConservative(vector<cv::Point> contour, double xc, double yc, double theta, double a, double b, double epsilon){
		double cosine2 = cos(-theta);
		double sine2 = sin(-theta);
		double x, y, xTrans, yTrans, xRel, yRel;
		double totalOutDist = 0;
		double totalInDist = 0;

		//caclulations not depending on x, y
		double exponent = 2.0 / epsilon;
		double exponentInv = epsilon / 2.0;
		double termOne = 1 / pow(a, exponent);

		//for each point of the contour:
		for (int i = 0; i < contour.size(); i++){
			x = contour.at(i).x;
			y = contour.at(i).y;

			//transform coordinates to the coordinatesystem of the superellipse
			xTrans = x - xc;
			yTrans = y - yc;
			xRel = (xTrans * cosine2 - yTrans * sine2);
			yRel = (xTrans * sine2 + yTrans * cosine2);

			//evaluate only in first quadrant
			xRel = fabs(xRel);
			yRel = fabs(yRel);

			//calculate the euclidean distance between the point and the point (xs, ys) on the superellipse
			double divOne = termOne + fabs(pow((yRel / (xRel*b)), exponent));
			double xs = pow((fabs(1 / (divOne))), exponentInv);

			//special numerical case: xRel near 0, calculation might result in dist INF
			if (xRel < 0.000001) {
				if (yRel < b) { //point on the inside of the superellipse
					xs = xRel + 1;
				}
				else { //point on the outside of the superellipse
					xs = xRel - 1;
				}
			}


			//test if point is inside or outside of the superellipse (everything evaluated in first quadrant!)
			if (xs < xRel) {
				//xRel outside of the superellipse -> NOT CONSERVATIVE
				return false;
			}
		}

		//all points inside of the superellipse -> CONSERVATIVE
		return true;

	}



	//operator overload to allow for printing of the struct contourAndSe

	std::ostream& operator<<(std::ostream& os, const se::contourAndSe& c)
	{
		os << c.offSetX << ',' << c.offSetY;

		//os << "contour size: " << c.contour.size() << "  offset: " <<  c.offSetX << ',' << c.offSetY;
		return os;
	}













}