#include <chrono>


#include "ellipseFit.h"
#include <ceres/ceres.h>


using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;

using namespace std;



vector<tuple<double,double>> getInput(string inputFile, int &width, int &height) {
	vector<tuple<double,double>> input;

	ifstream infile(inputFile);
	string dummy;
	infile >> dummy >> width;
	infile >> dummy >> height;


	double xIn, yIn;
	while (infile >> xIn >> yIn){
		tuple<double, double> point(xIn, yIn);
		input.push_back(point);
	}

	return input;
}







tuple<double,double> getCentroid(vector<tuple<double,double>> input) {
	double xSum = 0;
	double ySum = 0;
	double x, y;
	for (int i = 0; i < input.size(); i++) {
		x = get<0>(input.at(i));
		y = get<1>(input.at(i));
		xSum += x;
		ySum += y;
	}
	cout << "xSum: " << xSum << endl;
	cout << "ySum: " << xSum << endl;


	double xCentroid = xSum / double(input.size());
	double yCentroid = ySum / double(input.size());

	cout << "xCentroid: " << xCentroid << endl;
	cout << "yCentroid: " << yCentroid << endl;


	tuple<double, double> centroid(xCentroid, yCentroid);

	return centroid;
}




double dRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}



class DistanceFromCircleCost {
public:

	//store measured x- and y-coordinate that shoud be on the circle
	DistanceFromCircleCost(double xx, double yy) : xx_(xx), yy_(yy) {}

	//create functor evaluating the cost
	template <typename T> 
	bool operator()(const T* const xc, const T* const yc, const T* const theta, const T* const a, const T* const b, const T* const e, T* residual) const {
		T xTrans = T(xx_) - *xc + dRand(0.00000000001, 0.00000000009);
		T yTrans = T(yy_) - *yc + dRand(0.00000000001, 0.00000000009);

		T xRel = xTrans * cos(-*theta) - yTrans * sin(-*theta) + dRand(0.00000000001, 0.00000000009);
		T yRel = xTrans * sin(-*theta) + yTrans * cos(-*theta) + dRand(0.00000000001, 0.00000000009);
		

		//==================================

		//T firstTerm = (abs(xRel) / *a);
		//T secondTerm = (abs(yRel) / *b);
		//T exponent = 2.0 / *e;
		//residual[0] = abs(pow(firstTerm, exponent) + pow(secondTerm, exponent) - 1.0);

		//==================================


		//cout << "xc : " << *xc << endl;
		//cout << "yc : " << *yc << endl;
		//cout << "a : " << *a << endl;
		//cout << "b : " << *b << endl;
		//cout << "e : " << *e << endl;

		T temp3 = abs(yRel) / (abs(xRel) * *b);
		//cout << "temp3 : " << temp3 << endl;
		T tEMP1 = 2.0 / *e;
		//cout << "tEMP1 : " << tEMP1 << endl;
		T tEMP2 = pow(*a, tEMP1);
		//cout << "tEMP2 : " << tEMP2 << endl;
		T temp2 = 1.0 / tEMP2;
		//cout << "temp2 : " << temp2 << endl;
		T temp1 = temp2 + abs(pow(temp3, 2.0 / *e));
		//cout << "temp1 : " << temp1 << endl;
		T xs = pow(abs(1.0 / temp1), *e / 2.0);
		//cout << "xs : " << xs << endl;

		T ys = xs * (abs(yRel) / abs(xRel));
		//cout << "ys : " << ys << endl;

		residual[0] = (abs(xRel) - xs) + (abs(yRel) - ys);
		//cout << "residual : " << residual[0] << endl;

		return true;
	}

private:
	// The measured x,y coordinate that should be on the circle.
	double xx_, yy_;
};






int useCeres(string inputFile, string outputFile) {
  
	int width, height;
  vector<tuple<double,double>> input = getInput(inputFile, width, height);
  
  tuple<double, double> centroid = getCentroid(input);


  // Save initial values for comparison.
  double initial_xc = get<0>(centroid);
  double initial_yc = get<1>(centroid);
  double initial_theta = 0.0;
  double initial_a = 200.0;
  double initial_b = 200.0;
  double initial_e = 1.0;

  cout << "centroid: " << initial_xc << ", " << initial_yc << endl;


  double xc = initial_xc;
  double yc = initial_yc;
  double theta = initial_theta;
  double a = initial_a;
  double b = initial_b;
  double e = initial_e;

  Problem problem;

  double input_x;
  double input_y;

  for (int i = 0; i < input.size(); ++i) {
	  input_x = get<0>(input.at(i));
	  input_y = get<1>(input.at(i));
	  if (input_x == 0.0) input_x = dRand(0.00000000001, 0.00000000009);;
	  if (input_y == 0.0) input_y = dRand(0.00000000001, 0.00000000009);;
	  //cout << input_x << " " << input_y << endl;

	  CostFunction *cost =
		  new AutoDiffCostFunction<DistanceFromCircleCost, 1, 1, 1, 1, 1, 1, 1>(
		  new DistanceFromCircleCost(input_x, input_y));
	  problem.AddResidualBlock(cost, NULL, &xc, &yc, &theta, &a, &b, &e);
	  problem.SetParameterLowerBound(&a, 0, 0.001);
	  problem.SetParameterLowerBound(&b, 0, 0.001);
	  problem.SetParameterLowerBound(&e, 0, 0.001);
	  problem.SetParameterLowerBound(&theta, 0, -3.200);
	  problem.SetParameterUpperBound(&theta, 0, 3.200);
  }



  // Run the solver!
  Solver::Options options;
  options.max_solver_time_in_seconds = 15;
  options.num_threads = 10;
  options.max_num_iterations = 25;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = true;



  Solver::Summary summary;
  Solve(options, &problem, &summary);
  
  
  std::cout << summary.FullReport() << "\n";
  std::cout << "xc : " << initial_xc << " -> " << xc << "\n";
  std::cout << "yc : " << initial_yc << " -> " << yc << "\n";
  std::cout << "theta : " << initial_theta << " -> " << theta << "\n";
  std::cout << "a : " << initial_a << " -> " << a << "\n";
  std::cout << "b : " << initial_b << " -> " << b << "\n";
  std::cout << "e : " << initial_e << " -> " << e << "\n";


  ofstream outfile(outputFile);

  outfile << "w " << width << endl;
  outfile << "h " << height << endl;
  outfile << xc << " " << yc << " " << theta << " " << a << " " << b << " " << e << endl;



  return 0;
}
