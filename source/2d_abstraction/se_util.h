#ifndef SE_UTIL_H
#define SE_UTIL_H




namespace se {



	double evaluateFitImplicid(std::vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon);

	double evaluateFitEuclidean(std::vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon);

	bool isFitConservative(std::vector<int> contourVector, double xc, double yc, double theta, double a, double b, double epsilon);







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
















}




#endif /* SE_UTIL_H*/
