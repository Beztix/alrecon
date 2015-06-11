/**
*************************************************************************
*
* @file unused.cpp
*
* Class simply containg all written but currently unused functions to preserve them for possible later use.
*
************************************************************************/







//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//void appendEllipseToGlobalFile(string ellipseTextFile, string globalEllipseTextFile) {
//	ifstream infile(ellipseTextFile);
//	int width, height;
//	string dummy;
//	infile >> dummy >> width;
//	infile >> dummy >> height;
//
//	//Test if global ellipse file is initialized
//	ifstream readOutfile(globalEllipseTextFile);
//	int dummyInt;
//	if (!(readOutfile >> dummy >> dummyInt)) {
//		ofstream outfileInit(globalEllipseTextFile);
//		outfileInit << "w " << width << endl;
//		outfileInit << "h " << height << endl;
//	}
//
//	//append ellipse to global ellipse file
//	double xc, yc, theta, a, b, epsilon;
//	infile >> xc >> yc >> theta >> a >> b >> epsilon;
//
//	ofstream outfile(globalEllipseTextFile, std::ios_base::app);
//	outfile << xc << " " << yc << " " << theta << " " << a << " " << b << " " << epsilon << endl;
//}










//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//void appendEllipseToGlobalFileRonin(string ellipseTextFile, string globalEllipseTextFile) {
//	ifstream infile(ellipseTextFile);
//	int width = 1000;
//	int height = 1000;
//	int dummyInt;
//	string dummy;
//	infile >> dummy;
//	infile >> dummy >> dummyInt;
//
//	//Test if global ellipse file is initialized
//	ifstream readOutfile(globalEllipseTextFile);
//	if (!(readOutfile >> dummy)) {
//		ofstream outfileInit(globalEllipseTextFile);
//		outfileInit << "super" << endl;
//		outfileInit << "list: 0 " << endl;
//	}
//
//	//append ellipse to global ellipse file
//	double xc, yc, theta, a, b, epsilon, d;
//	infile >> dummy >> d >> xc >> yc >> d >> d >> d >> d >> a >> b >> theta >> epsilon >> d;
//
//	ofstream outfile(globalEllipseTextFile, std::ios_base::app);
//	outfile << "superellipse: 0.0 " << xc << " " << yc << " 0 0 0 0 " << a << " " << b << " " << theta << " " << epsilon << " 1" << endl;
//}









//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//void useCeresRecursive(string pixelFile, string contourFile, string ellipseTextFile, string id, string globalEllipseTextFile) {
//	double x, y, theta;
//	double maxFinalCost = useCeres(contourFile, ellipseTextFile, x, y, theta);
//	if (maxFinalCost > 10000) {
//		string id1 = id + "0";
//		string id2 = id + "1";
//		splitImage(pixelFile, x, y, theta, "file" + id1, "file" + id2);
//
//		pixelFileToImage("file" + id1, "file" + id1 + ".png");
//		pixelFileToImage("file" + id2, "file" + id2 + ".png");
//
//		getContours("file" + id1, "contourFile" + id1);
//		getContours("file" + id2, "contourFile" + id2);
//
//
//		pixelFileToImage("contourFile" + id1, "contourFile" + id1 + ".png");
//		pixelFileToImage("contourFile" + id2, "contourFile" + id2 + ".png");
//
//		useCeresRecursive("file" + id1, "contourFile" + id1, "ellipseFile" + id1, id1, globalEllipseTextFile);
//		processSuperellipsesFromTextfileCeres("ellipseFile" + id1, "ellipseFile" + id1 + ".png");
//
//		useCeresRecursive("file" + id2, "contourFile" + id2, "ellipseFile" + id2, id2, globalEllipseTextFile);
//		processSuperellipsesFromTextfileCeres("ellipseFile" + id2, "ellipseFile" + id2 + ".png");
//
//	}
//	else {
//		appendEllipseToGlobalFile(ellipseTextFile, globalEllipseTextFile);
//	}
//
//
//}








//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//void useRoninRecursive(string pixelFile, string contourFile, string ellipseTextFile, string id, string globalEllipseTextFile) {
//	
//	//convert filename strings to chararrays
//	char *contourFileChar = new char[contourFile.size() + 1];
//	contourFileChar[contourFile.size()] = 0;
//	memcpy(contourFileChar, contourFile.c_str(), contourFile.size());
//	char *ellipseTextFileChar = new char[ellipseTextFile.size() + 1];
//	ellipseTextFileChar[ellipseTextFile.size()] = 0;
//	memcpy(ellipseTextFileChar, ellipseTextFile.c_str(), ellipseTextFile.size());
//
//
//	//fit one superellipse to the contour contained in the contourFile
//	calculateEllipse2(contourFileChar, ellipseTextFileChar);
//	processSuperellipsesFromTextfile(ellipseTextFile, ellipseTextFile + ".png");
//
//	//evaluate the fit
//	double xc, yc, theta;
//	double currentValue = evaluateSuperellipse(contourFile, ellipseTextFile, xc, yc, theta);
//	
//	//if the fit is not good enough yet: split the input and fit two separate superellipses
//	if (currentValue > 100) {
//		string id1 = id + "0";
//		string id2 = id + "1";
//
//		splitImage(pixelFile, xc, yc, theta, "file" + id1, "file" + id2);
//		pixelFileToImage("file" + id1, "file" + id1 + ".png");
//		pixelFileToImage("file" + id2, "file" + id2 + ".png");
//
//
//		getContoursRonin("file" + id1, "contourFile" + id1);
//		getContoursRonin("file" + id2, "contourFile" + id2);
//		pixFileToImage("contourFile" + id1, "contourFile" + id1 + ".png");
//		pixFileToImage("contourFile" + id2, "contourFile" + id2 + ".png");
//
//		useRoninRecursive("file" + id1, "contourFile" + id1, "ellipseFile" + id1, id1, globalEllipseTextFile);
//		//processSuperellipsesFromTextfileCeres("ellipseFile" + id1, "ellipseFile" + id1 + ".png");
//
//		useRoninRecursive("file" + id2, "contourFile" + id2, "ellipseFile" + id2, id2, globalEllipseTextFile);
//		//processSuperellipsesFromTextfileCeres("ellipseFile" + id2, "ellipseFile" + id2 + ".png");
//
//	}
//
//	//if the fit is good enough: add the superellipse to the global file
//	else {
//		appendEllipseToGlobalFileRonin(ellipseTextFile, globalEllipseTextFile);
//	}
//}













//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//double input_sorted[1000] = {
//	63, 140,
//	63, 141,
//	63, 142,
//	63, 143,
//	64, 138,
//	64, 139,
//	64, 144,
//	65, 137,
//	65, 138,
//	65, 145,
//	66, 135,
//	66, 136,
//	66, 146,
//	67, 134,
//	67, 147,
//	68, 134,
//	68, 148,
//	69, 134,
//	69, 149,
//	69, 151,
//	69, 152,
//	69, 153,
//	69, 154,
//	69, 154,
//	69, 155,
//	69, 156,
//	70, 135,
//	70, 150,
//	70, 157,
//	70, 158,
//	70, 159,
//	70, 160,
//	71, 136,
//	71, 161,
//	71, 162,
//	71, 163,
//	72, 137,
//	72, 164,
//	72, 165,
//	72, 166,
//	72, 167,
//	72, 168,
//	73, 137,
//	73, 146,
//	73, 147,
//	73, 148,
//	73, 169,
//	73, 170,
//	74, 138,
//	74, 144,
//	74, 145,
//	74, 149,
//	74, 170,
//	75, 138,
//	75, 143,
//	75, 150,
//	75, 171,
//	76, 139,
//	76, 141,
//	76, 142,
//	76, 151,
//	76, 152,
//	76, 153,
//	76, 171,
//	77, 140,
//	77, 154,
//	77, 154,
//	77, 155,
//	77, 156,
//	77, 157,
//	77, 171,
//	78, 158,
//	78, 159,
//	78, 160,
//	78, 171,
//	79, 161,
//	79, 162,
//	79, 170,
//	80, 163,
//	80, 164,
//	80, 169,
//	80, 170,
//	81, 165,
//	81, 166,
//	81, 167,
//	81, 168,
//};


//vector<Point> points_sorted;
//for (int i = 0; i < 85; i++) {
//	Point p;
//	p.x = input_sorted[2 * i];
//	p.y = input_sorted[2 * i + 1];
//	points_sorted.emplace_back(p);
//}

//cout << "==================================" << endl;
//cout << "=======  Input by C++ Call  =======" << endl;
//for (int i = 0; i < points_sorted.size(); i++) {
//	Point p = points_sorted.at(i);
//	cout << p.x << " " << p.y << endl;
//}
//cout << "==================================" << endl;



//vector<Point> hull_sorted = convex_hull(points_sorted);

//cout << "==================================" << endl;
//cout << "=======  Hull by C++ Call  =======" << endl;
//for (int i = 0; i < hull_sorted.size(); i++) {
//	Point p = hull_sorted.at(i);
//	cout << p.x << " " << p.y << endl;
//}
//cout << "==================================" << endl;


//double p[1000 * 2];
//int n1, n2;
//int ch[1000];
//int nch;
//static char directions[] = "norm";
//double epsilon = 0.001;
//int i, j, save_i;
//double dx, dy, theta, cosine, sine, x1, y1, x2, y2, x3, y3, x4, y4, xo, yo, xc, yc;
//double area, min_area;
//double min_x, min_y, max_x, max_y;

//for (i = 0; i < 85; i++) {
//	p[i * 2] = input_sorted[i*2];
//	p[i * 2 + 1] = input_sorted[i*2+1];
//}
//n1 = 85;

//cout << "==================================" << endl;
//cout << "=======   Input by C Call   =======" << endl;
//for (int i = 0; i < 85; i++) {
//	cout << p[i*2] << " " << p[i*2+1] << endl;
//}
//cout << "==================================" << endl;


//external_convex_hull(p, n1, ch, &nch, directions, epsilon);

//cout << "==================================" << endl;
//cout << "=======   Hull by C Call   =======" << endl;
//for (int i = 0; i < nch; i++) {
//	n1 = ch[i];
//	x1 = input_sorted[n1 * 2];
//	y1 = input_sorted[n1 * 2 + 1];
//	cout << x1 << " " << y1 << endl;
//}
//cout << "==================================" << endl;

////printf("calculated ch \n");
////for (i = 0; i < nch; i++) {
////	n1 = ch[i];
////	n2 = (ch[i + 1] % 85);
////	if (n1 == n2) continue;
////	x1 = input_sorted[n1 * 2];
////	y1 = input_sorted[n1 * 2 + 1];
////	x2 = input_sorted[n2 * 2];
////	y2 = input_sorted[n2 * 2 + 1];
////	//printf("x1: %f, y1: %f  -  x2: %f, y2: %f \n", x1, y1, x2, y2);
////	
////	printf("%f,%f, %f,%f, \n", x1, y1, x2, y2);
////}

////double ch_sorted_by_main[1000] = {
////	63, 140, 64, 138,
////	64, 138, 66, 135,
////	66, 135, 67, 134,
////	67, 134, 69, 134,
////	69, 134, 75, 138,
////	75, 138, 77, 140,
////	77, 140, 81, 165,
////	81, 165, 81, 167,
////	81, 167, 80, 170,
////	80, 170, 78, 171,
////	78, 171, 75, 171,
////	75, 171, 73, 170,
////	73, 170, 72, 168,
////	72, 168, 63, 143,
////	63, 143, 63, 140,
////	63, 140, 0, 0,
////};

///*double data_sorted[1000] = {
//	63.000000, 140.000000, 64.000000, 138.000000,
//	64.000000, 138.000000, 66.000000, 135.000000,
//	66.000000, 135.000000, 67.000000, 134.000000,
//	67.000000, 134.000000, 69.000000, 134.000000,
//	69.000000, 134.000000, 75.000000, 138.000000,
//	75.000000, 138.000000, 77.000000, 140.000000,
//	77.000000, 140.000000, 81.000000, 165.000000,
//	81.000000, 165.000000, 80.000000, 163.000000,
//	80.000000, 163.000000, 78.000000, 158.000000,
//	78.000000, 158.000000, 75.000000, 138.000000,
//	75.000000, 138.000000, 73.000000, 137.000000,
//	73.000000, 137.000000, 72.000000, 137.000000,
//	72.000000, 137.000000, 63.000000, 140.000000,
//	63.000000, 140.000000, 0.000000, 0.000000,
//};

//double data_unsorted[1000] = {
//	63.000000, 140.000000, 64.000000, 138.000000,
//	64.000000, 138.000000, 66.000000, 135.000000,
//	66.000000, 135.000000, 67.000000, 134.000000,
//	67.000000, 134.000000, 69.000000, 149.000000,
//	69.000000, 149.000000, 75.000000, 171.000000,
//	75.000000, 171.000000, 77.000000, 171.000000,
//	77.000000, 171.000000, 81.000000, 168.000000,
//	81.000000, 168.000000, 80.000000, 170.000000,
//	80.000000, 170.000000, 78.000000, 171.000000,
//	78.000000, 171.000000, 75.000000, 171.000000,
//	75.000000, 171.000000, 73.000000, 169.000000,
//	73.000000, 169.000000, 72.000000, 164.000000,
//	72.000000, 164.000000, 63.000000, 140.000000,
//	63.000000, 140.000000, 0.000000, 0.000000,
//};*/

////input::renderImage("ch_sorted_by_main.png", 300, 300, 14, ch_sorted_by_main);
///*input::renderImage("ch_sorted_by_epsilon2.png", 300, 300, 14, data_sorted);
//input::renderImage("ch_unsorted_by_epsilon2.png", 300, 300, 14, data_unsorted);*/







//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//if (PREPARE) {
//	cout << "Preparing input for superellipses - Ronin" << endl;
//	int err = prepareInputForRonin(inputImage, reducedImgName, pixelFile, contourFile, reduction);
//	cout << "Prepared input from file " << inputImage << " - pixelFile is " << pixelFile << ", contourFile is "<< contourFile << endl;
//}


//if (COMPUTE) {
//	cout << "Calculating superellipses" << endl;
//	char *pixelFileChar = new char[pixelFile.size() + 1];
//	pixelFileChar[pixelFile.size()] = 0;
//	memcpy(pixelFileChar, pixelFile.c_str(), pixelFile.size());

//	char *ellipseTextFileChar = new char[ellipseTextFile.size() + 1];
//	ellipseTextFileChar[ellipseTextFile.size()] = 0;
//	memcpy(ellipseTextFileChar, ellipseTextFile.c_str(), ellipseTextFile.size());

//	useRoninRecursive(pixelFile, contourFile, ellipseTextFile, "", globalEllipseTextFile);
//	cout << "Calculated ellipses for file " << pixelFile << " to file " << ellipseTextFile << endl;

//	//evaluateSuperellipse(pixelFile, ellipseTextFile);
//}


//if (RENDER) {
//	cout << "Rendering superellipses" << endl;

//	int err = processSuperellipsesFromTextfile(globalEllipseTextFile, ellipseImage);


//	cout << "Rendered ellipses from file " << ellipseTextFile << " to image " << ellipseImage << endl;
//}








//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//void writePixelsToTextFileForRonin(string outputFile, int* pixels, int width, int height) {
//
//	ofstream myfile;
//	myfile.open(outputFile);
//	myfile << "pixel" << endl;
//	myfile << "list: 0" << endl;
//
//
//
//	//write the set pixels to file
//	for (int i = 0; i < height*width; i++) {
//		if (pixels[i] != 0){
//			int y = i / width;
//			int x = i % width;
//
//			myfile << to_string(x) << " " << to_string(y) << endl;
//		}
//	}
//	myfile << "-1 -1" << endl;
//
//	myfile.close();
//
//}












//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//void writePixelsToTextFileForCeres(string outputFile, int* pixels, int width, int height) {
//
//	ofstream myfile;
//	myfile.open(outputFile);
//	myfile << "w " << width << endl;
//	myfile << "h " << height << endl;
//
//	//write the set pixels to file
//	for (int i = 0; i < height*width; i++) {
//		if (pixels[i] != 0){
//			int y = i / width;
//			int x = i % width;
//
//			myfile << to_string(x) << " " << to_string(y) << endl;
//		}
//	}
//	myfile.close();
//
//}








//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//int prepareInputForRonin(string inputImage, string reducedImgName, string pixelFile, string contourFile, int reduction) {
//	int width;
//	int height;
//
//	//loading binary input image
//	int* pixels = input::loadPixelsFromImage(inputImage, width, height);
//	if (pixels == nullptr){
//		return -1;
//	}
//
//	writePixelsToTextFileForCeres(pixelFile, pixels, width, height);
//
//	//reducing pixels
//	int* reducedPixels = input::reducePixels(pixels, width, height, reduction, reducedImgName);
//
//	writePixelsToTextFileForRonin(contourFile, reducedPixels, width, height);
//	return 0;
//}















//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//int prepareInputForCeres(string inputImage, string reducedImgName, string outputFile, int reduction) {
//	int width;
//	int height;
//
//	//loading binary input image
//	int* pixels = input::loadPixelsFromImage(inputImage, width, height);
//	if (pixels == nullptr){
//		return -1;
//	}
//
//	//reducing pixels
//	int* reducedPixels = input::reducePixels(pixels, width, height, reduction, reducedImgName);
//
//	writePixelsToTextFileForCeres(outputFile, reducedPixels, width, height);
//	writePixelsToTextFileForCeres("allPixels", pixels, width, height);
//	return 0;
//}















//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################



////load the pixels from image
//int* pixelGrid = image_input::loadPixelsFromImage(inputImage, width, height);
//if (pixelGrid == nullptr) {
//	return -1;
//}

////extract the contours of the image using the own reduce function
//vector<int> contourVector = util::getContoursArray(pixelGrid, width, height);
//int no_contourPixels = int(contourVector.size() / 2);
//int* contourList = &contourVector[0];
//



////extract the contours of the image using OpenCV (implicitly perform connected components separation)
//vector<vector<cv::Point>> contoursPointsVector = image_input::getContoursFromImage(inputImage, width, height);


//int contoursCount = contoursPointsVector.size();
//vector<vector<double>> totalEllipsesVector;

////for each contour:
//for (int i = 0; i < contoursCount; i++) {

//	//transform the contoursPoints to a contourVector
//	vector<cv::Point> contourPoints = contoursPointsVector.at(i);
//	vector<int> contourVector;
//	for (int j = 0; j < contourPoints.size(); j++) {
//		contourVector.push_back(contourPoints.at(j).x);
//		contourVector.push_back(contourPoints.at(j).y);
//	}

//	//draw the contour
//	//image_output::pixelVectorToImage(contourVector, width, height, to_string(i) + "contour.png");

//	int* pixelGrid = util::getFilledPixelGridFromContour(contourPoints, width, height);

//	//use Ronin to fit superellipses to the contour
//	vector<vector<double>> contourEllipsesVector = useRoninRecursive(pixelGrid, contourVector, width, height);
//	contourVector.clear();

//	//draw the fitted superellipses
//	//int err = processSuperellipsesFromVector(contourEllipsesVector, to_string(i) + ellipseImage, width, height);

//	//add the superellipses of this contour to the vector of all superellipses
//	totalEllipsesVector.insert(totalEllipsesVector.end(), contourEllipsesVector.begin(), contourEllipsesVector.end());
//}




////recursively fit superellipses to the contours
//vector<vector<double>> ellipsesVector = useRoninRecursive(pixelGrid, contourVector, width, height);










//void splitContourToVectorsOld(vector<cv::Point> contourPoints, const int width, const int height, const double xc, const double yc, const double theta, vector<cv::Point> &splitPart1, vector<cv::Point> &splitPart2) {
//
//	const int numPixels = width * height;
//
//	cv::Point point1;
//	cv::Point point2;
//
//
//	//rotate 90 deg to split orthogonal to the ellipse orientation
//	double newTheta = theta + 1.57079;
//
//
//	//split line is horizontal
//	if (newTheta == 0) {
//		point1 = cv::Point(0, yc);
//		point2 = cv::Point(width, yc);
//	}
//	//split line is vertical
//	else if (theta < 0.001) {
//		point1 = cv::Point(xc, 0);
//		point2 = cv::Point(xc, height);
//	}
//	else {
//		//use y = m*x +t:
//		double m = tan(newTheta);	//m is tan(theta)
//		double t = yc - m*xc;		//calculate t by using the known point xc,yc
//
//		//calculate point1 on the left side of the image
//		int y1 = int(t);
//
//		//point1 is above the image -> calculate point1 on the top of the image
//		if (y1 > height) {
//			double x1d = (height - t) / m;
//			int x1 = (int)x1d;
//			point1 = cv::Point(x1, height);
//		}
//		//point1 is below the image -> calculate point1 on the bottom of the image
//		else if (y1 < 0) {
//			double x1d = (0 - t) / m;
//			int x1 = (int)x1d;
//			point1 = cv::Point(x1, 0);
//		}
//		//point1 is fine
//		else {
//			point1 = cv::Point(0, y1);
//		}
//
//
//
//		//calculate point2 on the right side of the image
//		int y2 = int(m * width + t);
//
//		//point2 is above the image -> calculate point2 on the top of the image
//		if (y2 > height) {
//			double x2d = (height - t) / m;
//			int x2 = (int)x2d;
//			point2 = cv::Point(x2, height);
//		}
//		//point2 is below the image -> calculate point2 on the bottom of the image
//		else if (y2 < 0) {
//			double x2d = (0 - t) / m;
//			int x2 = (int)x2d;
//			point2 = cv::Point(x2, 0);
//		}
//		//point2 is fine
//		else {
//			point2 = cv::Point(width, y2);
//		}
//
//	}
//
//	//test each contour point against the split line and add it to the corresponding split part
//	for (int i = 0; i < contourPoints.size(); i++) {
//		cv::Point testPoint = contourPoints.at(i);
//
//		if (isLeftOf(testPoint, point1, point2)){
//			splitPart1.push_back(testPoint);
//		}
//		else {
//			splitPart2.push_back(testPoint);
//		}
//	}
//
//	//rasterize the split line
//	vector<cv::Point> splitLine = util::bresenhamLine(point1, point2);
//
//	//test for each point of the splitLine if it is contained in the original contour
//	for (int i = 0; i < splitLine.size(); i++) {
//		cv::Point testPoint = splitLine.at(i);
//		double val = cv::pointPolygonTest(contourPoints, testPoint, false);
//		//point is inside or on the edge of the contour
//		if (val >= 0) {
//			//add the contained points of the split line to both splitParts
//			splitPart1.push_back(testPoint);
//			splitPart2.push_back(testPoint);
//		}
//	}
//
//}




