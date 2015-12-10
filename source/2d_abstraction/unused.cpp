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
















//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




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















//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################






//############################################
//##########          CGAL          ##########       
//############################################
//
//if (USE_CGAL) {
//
//	//++++++++++++++++++++++++++++
//	string inputName = "image";
//	int reduction = 5;
//	//++++++++++++++++++++++++++++
//
//	string inputImage = "images/" + inputName + ".png";
//	string reducedImgName = "as_cgal_" + inputName + "_reducedImage.png";
//	string outputImage = "as_cgal_" + inputName + "_alphaImage.png";
//
//	useCgal(inputImage, reducedImgName, outputImage, reduction);
//}








//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




//############################################
//##########        GPUAlpha        ##########       
//############################################
//
//if (USE_GPUALPHA) {
//
//	//++++++++++++++++++++++++++++
//	bool PREPARE = true;
//
//	string inputName = "1920x1080_high";
//	int reduction = 5;
//	//++++++++++++++++++++++++++++
//
//
//	string inputImage = "images/" + inputName + ".png";
//	string reducedImgName = "as_gpu_" + inputName + "_reducedImage.png";
//	string pixelFile = "as_gpu_" + inputName + "_pixels.txt";
//
//
//	if (PREPARE) {
//		cout << "Preparing input for GPUAlpha" << endl;
//		int err = prepareInputForAS(inputImage, reducedImgName, pixelFile, reduction);
//		cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
//	}
//
//}



//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//
//if (RECURSIVE) {
//	string ellipseImage = "se_rosin_rec_" + inputName + "_ellipseImage_" + to_string(quality) + ".png";
//	string combinedImage = "se_rosin_rec_" + inputName + "_combined" + to_string(quality) + ".png";
//
//	vector<se::superellipse> totalEllipsesVector;
//
//	//do the whole computation multiple times (performance averageing)
//	for (int i = 0; i < iterations; i++) {
//		totalEllipsesVector.clear();
//
//		//use recursive rosin fitting to fit superellipses to the contours
//
//		QueryPerformanceCounter(&t3);
//		totalEllipsesVector = startRosinRecursive(contours, width, height, quality);
//
//		QueryPerformanceCounter(&t4);
//		double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
//		cout << "computeDuration:     " << computeDuration << "ms" << endl;
//
//		totalComputeDuration = totalComputeDuration + computeDuration;
//	}
//
//	cout << endl;
//	cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
//	cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;
//
//	cout << endl;
//	cout << "quality used:                 " << quality << endl;
//	cout << "number of superellipses:      " << totalEllipsesVector.size() << endl;
//
//	//render the fitted superellipses to an image file
//	QueryPerformanceCounter(&t5);
//	int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);
//
//	//combine the original and the ellipse image
//	image_output::combineOriginalAndEllipseImage(inputImage, ellipseImage, combinedImage);
//
//	QueryPerformanceCounter(&t6);
//	double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
//	cout << "renderDuration:               " << renderDuration << "ms" << endl;
//}






//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//#######################################################
//##########        Superellipses Ceres        ##########       
//#######################################################

//if (USE_SUPERELLIPSES_CERES) {
//
//	//++++++++++++++++++++++++++++
//	bool PREPARE = true;
//	bool COMPUTE = true;
//	bool RENDER = true;
//
//	string inputName = "ellipsetest8";
//	int reduction = 100;
//	//++++++++++++++++++++++++++++
//
//	string inputImage = "images/" + inputName + ".png";
//	string reducedImgName = "se_ceres_" + inputName + "_reducedImage.png";
//	string pixelFile = "se_ceres_" + inputName + "_pixels.txt";
//	string ellipseTextFile = "se_ceres_" + inputName + "_textellipses";
//	string ellipseImage = "se_ceres_" + inputName + "_ellipseImage.png";
//
//
//	if (PREPARE) {
//		cout << "Preparing input for superellipses - Ceres" << endl;
//		//int err = prepareInputForCeres(inputImage, reducedImgName, pixelFile, reduction);
//		cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
//	}
//
//
//	if (COMPUTE) {
//		//double x, y, theta;
//		//useCeresRecursive("allPixels", pixelFile, ellipseTextFile, "", "globalEllipses");
//
//	}
//
//
//	if (RENDER) {
//		cout << "Rendering superellipses" << endl;
//		//int err = processSuperellipsesFromTextfileCeres("globalEllipses", "globalEllipses.png");
//		cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;
//
//	}
//
//
//
//
//}




//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


/*

rec::frust firstFrust(cv::Point3f(0.0, 0.0, 0.0), cv::Point3f(2.0, 0.0, 0.0), cv::Point3f(2.0, -2.0, 0.0), cv::Point3f(0.0, -2.0, 0.0),
cv::Point3f(0.0, 0.0, 4.0), cv::Point3f(2.0, 0.0, 4.0), cv::Point3f(2.0, -2.0, 4.0), cv::Point3f(0.0, -2.0, 4.0));

rec::frust secondFrust(cv::Point3f(-1.0, 0.0, -2.0), cv::Point3f(1.0, 0.0, -2.0), cv::Point3f(1.0, -2.0, -2.0), cv::Point3f(-1.0, -2.0, -2.0),
cv::Point3f(-1.0, 0.0, 2.0), cv::Point3f(1.0, 0.0, 2.0), cv::Point3f(1.0, -2.0, 2.0), cv::Point3f(-1.0, -2.0, 2.0));


bool intersect = rec::doFrustumsIntesect(firstFrust, secondFrust);

cout << intersect << endl;






file_camera cam(0);

sensor sens(0, cam.image_size, sensor::projection_pinhole_distort);

sens.set_pinhole_distort(cam.world_to_device_pinhole_distort_, cam.pinhole_distort_center_, cam.pinhole_distort_focus_, cam.distort_r1_, cam.distort_r2_, cam.distort_t1_, cam.distort_t2_);



std::vector<viral_core::vector> projectedVectors;
std::vector<int> pixels;

for (int i = 0; i < (2200+2000)/20; i++) {
int x = -2000 + 20 * i;
viral_core::vector vec((float)x, 0, 0);

viral_core::vector projVec = sens.project(vec);
projectedVectors.emplace_back(projVec);
pixels.emplace_back((int)projVec.x);
pixels.emplace_back((int)projVec.y);
}
image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTestx.png");


for (int i = 0; i < (2200 + 2200) / 20; i++) {
int y = -2200 + 20 * i;
viral_core::vector vec(0, (float)y, 0);

viral_core::vector projVec = sens.project(vec);
projectedVectors.emplace_back(projVec);
pixels.emplace_back((int)projVec.x);
pixels.emplace_back((int)projVec.y);
}
image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTesty.png");


for (int i = 0; i < (880 + 1600) / 20; i++) {
int z = -880 + 20 * i;
viral_core::vector vec(0, 0, (float)z);

viral_core::vector projVec = sens.project(vec);
projectedVectors.emplace_back(projVec);
pixels.emplace_back((int)projVec.x);
pixels.emplace_back((int)projVec.y);
}
image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTestz.png");



cout << "test" << endl;


*/





//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


/*




///////// START RENDERING-DEMO

viral_core::engine e("alrecon");
viral_core::step_timer t(0.02f, 0.5f);


// Resourcen
viral_core::shared_pointer<viral_core::render_model_id> model_id
(new viral_core::render_model_id("my_model"));
viral_core::shared_pointer<viral_core::render_shader_id> shader_id
(new viral_core::render_shader_id("my_shader"));
viral_core::shared_pointer<viral_core::render_material_id> material_id
(new viral_core::render_material_id("my_material"));



// Scene
viral_core::shared_pointer<viral_core::render_puppet_id> puppet_id
(new viral_core::render_puppet_id("my_puppet"));
viral_core::shared_pointer<viral_core::render_light_id> light_id
(new viral_core::render_light_id("my_light"));
viral_core::shared_pointer<viral_core::render_scene_id> scene_id
(new viral_core::render_scene_id("my_scene"));

// Process
viral_core::shared_pointer<viral_core::render_layer_id> layer_id
(new viral_core::render_layer_id("my_layer"));
viral_core::shared_pointer<viral_core::render_canvas_id> canvas_id
(new viral_core::render_canvas_id("my_canvas"));
viral_core::shared_pointer<viral_core::render_process_id> process_id
(new viral_core::render_process_id("my_process"));


// Put together geometry.
viral_core::auto_pointer<viral_core::mesh> geometry_mesh
(new viral_core::mesh());
geometry_mesh->allocate_vertices(4);
geometry_mesh->allocate_triangles(2);

geometry_mesh->vertices()[0] = viral_core::vector(-1, -1, 0);
geometry_mesh->vertices()[1] = viral_core::vector(1, -1, 0);
geometry_mesh->vertices()[2] = viral_core::vector(1, 1, 0);
geometry_mesh->vertices()[3] = viral_core::vector(-1, 1, 0);

geometry_mesh->triangles()[0] = viral_core::mesh_triangle(0, 2, 1);
geometry_mesh->triangles()[1] = viral_core::mesh_triangle(0, 3, 2);

viral_core::auto_pointer<viral_core::mesh_stream_vector> geometry_mesh_normals
(new viral_core::mesh_stream_vector());

geometry_mesh_normals->resize(4);
(*geometry_mesh_normals)[0] = viral_core::vector(0, 0, -1);
(*geometry_mesh_normals)[1] = viral_core::vector(0, 0, -1);
(*geometry_mesh_normals)[2] = viral_core::vector(0, 0, -1);
(*geometry_mesh_normals)[3] = viral_core::vector(0, 0, -1);

geometry_mesh->insert_vertex_stream
(viral_core::mesh_stream_registry::normal_stream_name,
geometry_mesh_normals);

viral_core::auto_pointer<viral_core::model> geometry
(new viral_core::model());
geometry->insert_group("model_group", geometry_mesh);
geometry->rebuild_boundings();
geometry->validate();


// Fill data structures for later render-side objects.
viral_core::render_model_data model_data;
model_data.geometry = geometry;

viral_core::render_shader_data shader_data;
shader_data.fragment_shader =
e.files().open_file("shaders/shader.glfs", viral_core::file::read_only)->read_text();
shader_data.vertex_shader =
e.files().open_file("shaders/shader.glvs", viral_core::file::read_only)->read_text();

viral_core::render_material_data material_data;
material_data.ambient_color = viral_core::color(0, 0, 0, 1);
material_data.diffuse_color = viral_core::color(0, 1, 1, 1);
material_data.cull = viral_core::render_material_data::cull_none;
material_data.shader = shader_id;

viral_core::render_puppet_data puppet_data;
puppet_data.position = viral_core::vector(0, 0, 16);
puppet_data.model = model_id;
puppet_data.materials.insert("model_group", material_id);

viral_core::render_light_data light_data;
light_data.emitter = viral_core::render_light_data::emitter_parallel;
light_data.orientation =
viral_core::rotation(viral_core::vector(1, 0, 0), -30) *
viral_core::rotation(viral_core::vector(0, 1, 0), 30);

viral_core::render_scene_data scene_data;
scene_data.objects.insert(puppet_id);
scene_data.objects.insert(light_id);

viral_core::render_layer_data layer_data;
layer_data.scene = scene_id;
layer_data.background_color = viral_core::color(0.2f, 0, 0, 0);

viral_core::render_canvas_data canvas_data;
canvas_data.layers.insert(layer_id);
canvas_data.copy_to_window = true;

viral_core::render_process_data process_data;
process_data.canvases.insert(canvas_id);
process_data.show_results = true;


// Commit data structures to render-side objects with appropriate ID.
viral_core::auto_pointer<viral_core::render_command_queue> q
(new viral_core::render_command_queue());
q->commit(model_data, model_id);
q->commit(shader_data, shader_id);
q->commit(material_data, material_id);
q->commit(puppet_data, puppet_id);
q->commit(light_data, light_id);
q->commit(scene_data, scene_id);
q->commit(layer_data, layer_id);
q->commit(canvas_data, canvas_id);
q->commit(process_data, process_id);
e.renderer().execute(q);

e.window().set_display
(viral_core::vector2f(50, 50), viral_core::vector2f(800, 600), 0);

while (true)
{
t.try_sleep();
t.update();

if (!t.has_next_timestep())
continue;


// Logik
while (t.next_timestep())
{
e.window().update();
e.input().update();

if (e.window().close_flag())
return 0;

update_camera(e.window(), e.input(), layer_data.cam);

// Eigener LOGIK-CODE
// Hier sind (mindestens) t.timestep_seconds vergangen
}


// Rendering
if (!e.rtask().has_pending_queue())
{
// Put together render commands.
viral_core::auto_pointer<viral_core::render_command_queue> q
(new viral_core::render_command_queue());

layer_data.cam.aspect_y_to_x =
e.window().size().y /
e.window().size().x;
q->commit(layer_data, layer_id);

canvas_data.pixel_size =
canvas_data.target_window_pixels.size =
e.window().size().to_vector2i();
q->commit(canvas_data, canvas_id);

// Eigener RENDERING-CODE
// Hier sind (mindestens) t.timestep_seconds vergangen

q->render(process_id);
e.rtask().execute(q);
}
}





///////// END RENDERING-DEMO



*/







//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

///**
//*************************************************************************
//*
//* @file as_cgal.cpp
//*
//* Class for using CGAL to calculate alpha shapes, implementation.
//*
//************************************************************************/
//
//#include "as_cgal.h"
//
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <list>
//#include <chrono>
//
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/algorithm.h>
//#include <CGAL/Delaunay_triangulation_2.h>
//
//#include "Alpha_shape_2.h"
//#include "image_input.h"
//#include "util.h"
//
//
//using namespace cv;
//using namespace std;
//
//
//
//typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef K::FT FT;
//typedef K::Point_2  Point_2;
//typedef K::Segment_2  Segment;
//typedef CGAL::Alpha_shape_vertex_base_2<K> Vb;
//typedef CGAL::Alpha_shape_face_base_2<K>  Fb;
//typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
//typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation_2;
//typedef CGAL::Alpha_shape_2<Triangulation_2>  Alpha_shape_2;
//typedef Alpha_shape_2::Alpha_shape_edges_iterator Alpha_shape_edges_iterator;
//
//
//
//
//template <class OutputIterator>
//void
//alpha_edges(const Alpha_shape_2&  A,
//OutputIterator out)
//{
//	for (Alpha_shape_edges_iterator it = A.alpha_shape_edges_begin();
//		it != A.alpha_shape_edges_end();
//		++it){
//		*out++ = A.segment(*it);
//	}
//}
//
//
//
/////converts set pixels represented by an integer array to a list of Point_2
//
//list<Point_2> convertPixelsToPoints(vector<int> pixels, int width, int height) {
//	list<Point_2> points;
//
//	//create corresponding points to set pixels
//	for (int i = 0; i < height*width; i++) {
//		if (pixels[i] != 0){
//			int y = i / width;
//			int x = i % width;
//
//			Point_2 p = Point_2(x, y);
//			points.emplace_back(p);
//		}
//	}
//
//	return points;
//}
//
//
//
//
//
//
//
/////creates an image out of a given vector of segments
///**
//* This method creates an image of the given width and height at the given location/name and writes the given segments into it.
//* End points of the segments are drawn in red, the inner parts of the segments are drawn in light blue (background is black).
//*/
//
//void writeSegmentsToImage(string imageLocation, std::vector<Segment> segments, int width, int height) {
//
//	Mat alphaImage = Mat(height, width, CV_8UC3);
//
//	for (int y = 0; y < height; y++) {
//		for (int x = 0; x < width; x++) {
//			alphaImage.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
//		}
//	}
//
//	Point_2 p1;
//	Point_2 p2;
//	Point point1;
//	Point point2;
//	Segment seg;
//	Vec3b color;
//
//	// for each segment mark its end points in the image
//	for (std::vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it) {
//		seg = *it;
//		p1 = seg.source();
//		p2 = seg.target();
//		point1 = Point(int(p1.x()), int(p1.y()));
//		point2 = Point(int(p2.x()), int(p2.y()));
//
//		//draw the line segment (light blue)
//		line(alphaImage, point1, point2, Scalar(255, 200, 20), 1, 8, 0);
//
//		//draw the end points of the segment (red)
//		alphaImage.at<Vec3b>(point1) = Vec3b(0, 50, 255);
//		alphaImage.at<Vec3b>(point2) = Vec3b(0, 50, 255);
//
//
//	}
//
//	imwrite(imageLocation, alphaImage);
//}
//
//
//
//
//
//
//// Calls the utility methods to load points from an image, calculates the alpha shape with a given parameter and save the calculated shapes to an image
///**
//* This method performs all steps needed to evaluate a binary image using alpha shapes:
//* The binary input image is first loaded using OpenCV, the interior pixels of the contained shapes are then optionally reduced
//* to reduce computational load by alpha shape computation.
//* The alpha shape is afterwards computed using CGAL and the computed segments are written to a new image using OpenCV.
//* The time needed for each step is evaluated and printed on console.
//*/
//
//int useCgal(string inputImage, string reducedImgName, string outputImage, int reduction)
//{
//	int width;
//	int height;
//	auto startTime = chrono::high_resolution_clock::now();
//
//	//loading binary input image
//	int* pixels = image_input::loadPixelGridFromImage(inputImage, width, height);
//	if (pixels == nullptr){
//		return -1;
//	}
//	auto loadTime = chrono::high_resolution_clock::now();
//
//
//	//reducing pixels
//	vector<int> reducedPixels = util::reducePixels(pixels, width, height, reduction, reducedImgName);
//	auto reduceTime = chrono::high_resolution_clock::now();
//
//
//	//converting pixels
//	list<Point_2> points = convertPixelsToPoints(reducedPixels, width, height);
//	auto convertTime = chrono::high_resolution_clock::now();
//
//	std::cout << "Image dimensions: " << width << " x " << height << std::endl;
//	std::cout << "list size:" << points.size() << std::endl;
//
//
//	//computing alpha shape
//	Alpha_shape_2 A(points.begin(), points.end(), FT(100), Alpha_shape_2::GENERAL);
//	std::vector<Segment> segments;
//	alpha_edges(A, std::back_inserter(segments));
//	std::cout << "Alpha Shape computed: there are " << segments.size() << " alpha shape edges" << std::endl;
//	auto alphaTime = chrono::high_resolution_clock::now();
//
//
//	//writing alpha shape image
//	writeSegmentsToImage(outputImage, segments, width, height);
//	auto writeImageTime = chrono::high_resolution_clock::now();
//
//
//
//
//	//evaluation time elapsed
//	auto loadDuration = loadTime - startTime;
//	auto reduceDuration = reduceTime - loadTime;
//	auto convertDuration = convertTime - reduceTime;
//	auto alphaDuration = alphaTime - convertTime;
//	auto writeImageDuration = writeImageTime - alphaTime;
//
//	cout << "loadDuration:     " << chrono::duration_cast<chrono::microseconds>(loadDuration).count() << endl;
//	cout << "reduceDuration:   " << chrono::duration_cast<chrono::microseconds>(reduceDuration).count() << endl;
//	cout << "convertDuration:  " << chrono::duration_cast<chrono::microseconds>(convertDuration).count() << endl;
//	cout << "alphaDuration:    " << chrono::duration_cast<chrono::microseconds>(alphaDuration).count() << endl;
//	cout << "writeImageDuration:    " << chrono::duration_cast<chrono::microseconds>(writeImageDuration).count() << endl;
//
//	return 0;
//}

















//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

///**
//*************************************************************************
//*
//* @file as_gpu_input.cpp
//*
//* Class for preparing the input for GPUAlpha, implementation.
//*
//************************************************************************/
//
//#include "as_gpu_input.h"
//
//#include <iostream>
//#include <fstream>
//
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//
//#include "image_input.h"
//#include "util.h"
//
//
//using namespace cv;
//using namespace std;
//
//
//
//
//
//void writePixelsToTextFileForAS(string outputFile, vector<int> pixels, int width, int height) {
//	int totalNum = 0;
//
//	//calculate number of set pixels
//	for (int i = 0; i < height*width; i++) {
//		if (pixels[i] != 0){
//			totalNum++;
//		}
//	}
//
//	ofstream myfile;
//	myfile.open(outputFile);
//	myfile << to_string(totalNum) << endl;
//
//	int num = 0;
//
//	//write the set pixels to file
//	for (int i = 0; i < height*width; i++) {
//		if (pixels[i] != 0){
//			int y = i / width;
//			int x = i % width;
//
//			myfile << to_string(num) << " " << to_string(x) << " " << to_string(y) << endl;
//			num++;
//		}
//	}
//
//	myfile.close();
//
//}
//
//
//
//
//
//int prepareInputForAS(string inputImage, string reducedImgName, string outputFile, int reduction) {
//	int width;
//	int height;
//
//	//loading binary input image
//	int* pixels = image_input::loadPixelGridFromImage(inputImage, width, height);
//	if (pixels == nullptr){
//		return -1;
//	}
//
//	//reducing pixels
//	vector<int> reducedPixels = util::reducePixels(pixels, width, height, reduction, reducedImgName);
//
//	//write pixels to textfile in Format used by GPUAlpha
//	writePixelsToTextFileForAS(outputFile, reducedPixels, width, height);
//
//	return 0;
//}














//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//vector<vector<cv::Point2i>> getBlobsFromImage(string imageLocation, int &width, int &height) {
//
//	Mat img = imread(imageLocation, 0);
//	if (!img.data) {
//		std::cout << "File not found" << std::endl;
//	}
//
//	//get size of the image
//	Size size = img.size();
//	height = size.height;
//	width = size.width;
//
//	vector<vector<cv::Point2i>> blobs;
//	Mat binary;
//	threshold(img, binary, 0.0, 1.0, cv::THRESH_BINARY);
//
//	blob::FindBlobs(binary, blobs);
//
//	int type1 = img.type();
//	int type2 = binary.type();
//
//
//
//	return blobs;
//}










//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//void FindBlobs(const cv::Mat &binary, std::vector < std::vector<cv::Point2i> > &blobs)
//{
//	blobs.clear();
//
//	// Fill the label_image with the blobs
//	// 0  - background
//	// 1  - unlabelled foreground
//	// 2+ - labelled foreground
//
//	cv::Mat label_image;
//	binary.convertTo(label_image, CV_32SC1);
//
//	int label_count = 2; // starts at 2 because 0,1 are used already
//
//	for (int y = 0; y < label_image.rows; y++) {
//		int *row = (int*)label_image.ptr(y);
//		for (int x = 0; x < label_image.cols; x++) {
//			if (row[x] != 1) {
//				continue;
//			}
//
//			cv::Rect rect;
//			cv::floodFill(label_image, cv::Point(x, y), label_count, &rect, 0, 0, 4);
//
//			std::vector <cv::Point2i> blob;
//
//			for (int i = rect.y; i < (rect.y + rect.height); i++) {
//				int *row2 = (int*)label_image.ptr(i);
//				for (int j = rect.x; j < (rect.x + rect.width); j++) {
//					if (row2[j] != label_count) {
//						continue;
//					}
//
//					blob.push_back(cv::Point2i(j, i));
//				}
//			}
//
//			blobs.push_back(blob);
//
//			label_count++;
//		}
//	}
//
//
//}



//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################

//#include <chrono>
//
//
//#include "ellipseFit.h"
//#include <ceres/ceres.h>
//
//using ceres::AutoDiffCostFunction;
//using ceres::CostFunction;
//using ceres::Problem;
//using ceres::Solver;
//using ceres::Solve;
//
//using namespace std;
//
//
//
//vector<tuple<double, double>> getInput(string inputFile, int &width, int &height) {
//	vector<tuple<double, double>> input;
//
//	ifstream infile(inputFile);
//	string dummy;
//	infile >> dummy >> width;
//	infile >> dummy >> height;
//
//
//	double xIn, yIn;
//	while (infile >> xIn >> yIn){
//		tuple<double, double> point(xIn, yIn);
//		input.push_back(point);
//	}
//
//	return input;
//}
//
//
//
//
//
//
//
//tuple<double, double> getCentroid(vector<tuple<double, double>> input) {
//	double xSum = 0;
//	double ySum = 0;
//	double x, y;
//	for (int i = 0; i < input.size(); i++) {
//		x = get<0>(input.at(i));
//		y = get<1>(input.at(i));
//		xSum += x;
//		ySum += y;
//	}
//	cout << "xSum: " << xSum << endl;
//	cout << "ySum: " << xSum << endl;
//
//
//	double xCentroid = xSum / double(input.size());
//	double yCentroid = ySum / double(input.size());
//
//	cout << "xCentroid: " << xCentroid << endl;
//	cout << "yCentroid: " << yCentroid << endl;
//
//
//	tuple<double, double> centroid(xCentroid, yCentroid);
//
//	return centroid;
//}
//
//
//
//
//double dRand(double fMin, double fMax)
//{
//	double f = (double)rand() / RAND_MAX;
//	return fMin + f * (fMax - fMin);
//}
//
//
//
//class DistanceFromCircleCost {
//public:
//
//	//store measured x- and y-coordinate that shoud be on the circle
//	DistanceFromCircleCost(double xx, double yy) : xx_(xx), yy_(yy) {}
//
//	//create functor evaluating the cost
//	template <typename T>
//	bool operator()(const T* const xc, const T* const yc, const T* const theta, const T* const a, const T* const b, const T* const e, T* residual) const {
//		T xTrans = T(xx_) - *xc + dRand(0.00000000001, 0.00000000009);
//		T yTrans = T(yy_) - *yc + dRand(0.00000000001, 0.00000000009);
//
//		T xRel = xTrans * cos(-*theta) - yTrans * sin(-*theta) + dRand(0.00000000001, 0.00000000009);
//		T yRel = xTrans * sin(-*theta) + yTrans * cos(-*theta) + dRand(0.00000000001, 0.00000000009);
//
//
//		//==================================
//
//		//T firstTerm = (abs(xRel) / *a);
//		//T secondTerm = (abs(yRel) / *b);
//		//T exponent = 2.0 / *e;
//		//residual[0] = abs(pow(firstTerm, exponent) + pow(secondTerm, exponent) - 1.0);
//
//		//==================================
//
//
//		//cout << "xc : " << *xc << endl;
//		//cout << "yc : " << *yc << endl;
//		//cout << "a : " << *a << endl;
//		//cout << "b : " << *b << endl;
//		//cout << "e : " << *e << endl;
//
//		T temp3 = abs(yRel) / (abs(xRel) * *b);
//		//cout << "temp3 : " << temp3 << endl;
//		T tEMP1 = 2.0 / *e;
//		//cout << "tEMP1 : " << tEMP1 << endl;
//		T tEMP2 = pow(*a, tEMP1);
//		//cout << "tEMP2 : " << tEMP2 << endl;
//		T temp2 = 1.0 / tEMP2;
//		//cout << "temp2 : " << temp2 << endl;
//		T temp1 = temp2 + abs(pow(temp3, 2.0 / *e));
//		//cout << "temp1 : " << temp1 << endl;
//		T xs = pow(abs(1.0 / temp1), *e / 2.0);
//		//cout << "xs : " << xs << endl;
//
//		T ys = xs * (abs(yRel) / abs(xRel));
//		//cout << "ys : " << ys << endl;
//
//		residual[0] = (abs(xRel) - xs) + (abs(yRel) - ys);
//		//cout << "residual : " << residual[0] << endl;
//
//		return true;
//	}
//
//private:
//	// The measured x,y coordinate that should be on the circle.
//	double xx_, yy_;
//};
//
//
//
//
//
//
//double useCeres(string inputFile, string outputFile, double &xOut, double &yOut, double &thetaOut) {
//
//	int width, height;
//	vector<tuple<double, double>> input = getInput(inputFile, width, height);
//
//	tuple<double, double> centroid = getCentroid(input);
//
//
//	// Save initial values for comparison.
//	double initial_xc = get<0>(centroid);
//	double initial_yc = get<1>(centroid);
//	double initial_theta = 0.0;
//	double initial_a = 200.0;
//	double initial_b = 200.0;
//	double initial_e = 1.0;
//
//	cout << "centroid: " << initial_xc << ", " << initial_yc << endl;
//
//
//	double xc = initial_xc;
//	double yc = initial_yc;
//	double theta = initial_theta;
//	double a = initial_a;
//	double b = initial_b;
//	double e = initial_e;
//
//	Problem problem;
//
//	double input_x;
//	double input_y;
//
//	for (int i = 0; i < input.size(); ++i) {
//		input_x = get<0>(input.at(i));
//		input_y = get<1>(input.at(i));
//		if (input_x == 0.0) input_x = dRand(0.00000000001, 0.00000000009);;
//		if (input_y == 0.0) input_y = dRand(0.00000000001, 0.00000000009);;
//		//cout << input_x << " " << input_y << endl;
//
//		CostFunction *cost =
//			new AutoDiffCostFunction<DistanceFromCircleCost, 1, 1, 1, 1, 1, 1, 1>(
//			new DistanceFromCircleCost(input_x, input_y));
//		problem.AddResidualBlock(cost, NULL, &xc, &yc, &theta, &a, &b, &e);
//		problem.SetParameterLowerBound(&a, 0, 0.001);
//		problem.SetParameterLowerBound(&b, 0, 0.001);
//		problem.SetParameterLowerBound(&e, 0, 0.001);
//		problem.SetParameterLowerBound(&theta, 0, -3.200);
//		problem.SetParameterUpperBound(&theta, 0, 3.200);
//	}
//
//
//
//	// Run the solver!
//	Solver::Options options;
//	options.max_solver_time_in_seconds = 5;
//	options.num_threads = 10;
//	options.max_num_iterations = 25;
//	options.linear_solver_type = ceres::DENSE_QR;
//	options.minimizer_progress_to_stdout = false;
//
//
//
//	Solver::Summary summary;
//	Solve(options, &problem, &summary);
//
//
//	if (a < b) {
//		double temp = a;
//		a = b;
//		b = temp;
//
//		theta += 1.5707963268;
//	}
//
//
//	std::cout << summary.FullReport() << "\n";
//	std::cout << "xc : " << initial_xc << " -> " << xc << "\n";
//	std::cout << "yc : " << initial_yc << " -> " << yc << "\n";
//	std::cout << "theta : " << initial_theta << " -> " << theta << "\n";
//	std::cout << "a : " << initial_a << " -> " << a << "\n";
//	std::cout << "b : " << initial_b << " -> " << b << "\n";
//	std::cout << "e : " << initial_e << " -> " << e << "\n";
//
//
//
//	ofstream outfile(outputFile);
//
//	outfile << "w " << width << endl;
//	outfile << "h " << height << endl;
//	outfile << xc << " " << yc << " " << theta << " " << a << " " << b << " " << e << endl;
//
//	xOut = xc;
//	yOut = yc;
//	thetaOut = theta;
//
//
//	return summary.final_cost;
//}








//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################


//
////calculates an axis aligned bounding box of the intersection of two pyramids by calculating all possible corners of the intersection object and building an aabb around it
//bool computePyramidIntersectionBoundingBoxInWorkspace(rec::pyramid firstPyramid, rec::pyramid secondPyramid, rec::aabb workspace, rec::aabb &result) {
//
//	std::vector<viral_core::vector> allIntersectionPoints;
//	std::vector<viral_core::vector> intersectionPointsTrianglesFirstEdgesSecond;
//	std::vector<viral_core::vector> intersectionPointsTrianglesSecondEdgesFirst;
//
//	viral_core::triangle currentTriangle;
//
//
//	//=== process triangles of first pyramid with edges of second pyramid ===
//
//	//generate edges of second pyramid
//	viral_core::vector secondPyramidEdgeDirections[4];
//	secondPyramidEdgeDirections[0] = (secondPyramid.corners[rec::pyramid::bot_left] - secondPyramid.corners[rec::pyramid::apex]).normalized();
//	secondPyramidEdgeDirections[1] = (secondPyramid.corners[rec::pyramid::bot_right] - secondPyramid.corners[rec::pyramid::apex]).normalized();
//	secondPyramidEdgeDirections[2] = (secondPyramid.corners[rec::pyramid::top_right] - secondPyramid.corners[rec::pyramid::apex]).normalized();
//	secondPyramidEdgeDirections[3] = (secondPyramid.corners[rec::pyramid::top_left] - secondPyramid.corners[rec::pyramid::apex]).normalized();
//
//
//	//generate triangles of first pyramid
//	viral_core::triangle firstPyramidTriangles[4];
//	firstPyramidTriangles[0] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::bot_left], firstPyramid.corners[rec::pyramid::bot_right]);
//	firstPyramidTriangles[1] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::bot_right], firstPyramid.corners[rec::pyramid::top_right]);
//	firstPyramidTriangles[2] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::top_right], firstPyramid.corners[rec::pyramid::top_left]);
//	firstPyramidTriangles[3] = viral_core::triangle(firstPyramid.corners[rec::pyramid::apex], firstPyramid.corners[rec::pyramid::top_left], firstPyramid.corners[rec::pyramid::bot_left]);
//
//
//	//intersect all edges of the second pyramid with the triangles of the first pyramid and save the intersection points
//	for (viral_core::triangle currentTriangle : firstPyramidTriangles) {
//		for (viral_core::vector currentEdgeDirection : secondPyramidEdgeDirections) {
//			viral_core::vector hitPoint;
//			float hitDistance;
//			if (currentTriangle.intersect_line(secondPyramid.corners[rec::pyramid::apex], currentEdgeDirection, hitPoint, hitDistance)) {
//
//				//standard case: add intersection point to the list of all intersection points
//				if (hitDistance > 0) {
//					allIntersectionPoints.push_back(hitPoint);
//					intersectionPointsTrianglesFirstEdgesSecond.push_back(hitPoint);
//				}
//				//intersection point is outside of the frustum (on the "wrong" side of the apex) -> intersection of the frustums is inifinite 
//				else {
//					//calculate intersection point of the ray with the workspace bounding box
//					hitPoint = util::calculateIntersectionPointRayInAABBwithAABB(secondPyramid.corners[rec::pyramid::apex], currentEdgeDirection, workspace);
//					allIntersectionPoints.push_back(hitPoint);
//					intersectionPointsTrianglesFirstEdgesSecond.push_back(hitPoint);
//				}
//			}
//		}
//	}
//
//
//
//	//=== process triangles of second pyramid with edges of first pyramid ===
//
//	//generate edges of first pyramid
//	viral_core::vector firstPyramidEdgeDirections[4];
//	firstPyramidEdgeDirections[0] = (firstPyramid.corners[rec::pyramid::bot_left] - firstPyramid.corners[rec::pyramid::apex]).normalized();
//	firstPyramidEdgeDirections[1] = (firstPyramid.corners[rec::pyramid::bot_right] - firstPyramid.corners[rec::pyramid::apex]).normalized();
//	firstPyramidEdgeDirections[2] = (firstPyramid.corners[rec::pyramid::top_right] - firstPyramid.corners[rec::pyramid::apex]).normalized();
//	firstPyramidEdgeDirections[3] = (firstPyramid.corners[rec::pyramid::top_left] - firstPyramid.corners[rec::pyramid::apex]).normalized();
//
//
//	//generate triangles of second pyramid
//	viral_core::triangle secondPyramidTriangles[4];
//	secondPyramidTriangles[0] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::bot_left], secondPyramid.corners[rec::pyramid::bot_right]);
//	secondPyramidTriangles[1] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::bot_right], secondPyramid.corners[rec::pyramid::top_right]);
//	secondPyramidTriangles[2] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::top_right], secondPyramid.corners[rec::pyramid::top_left]);
//	secondPyramidTriangles[3] = viral_core::triangle(secondPyramid.corners[rec::pyramid::apex], secondPyramid.corners[rec::pyramid::top_left], secondPyramid.corners[rec::pyramid::bot_left]);
//
//
//	//intersect all edges of the first pyramid with the triangles of the second pyramid and save the intersection points
//	for (viral_core::triangle currentTriangle : secondPyramidTriangles) {
//		for (viral_core::vector currentEdgeDirection : firstPyramidEdgeDirections) {
//			viral_core::vector hitPoint;
//			float hitDistance;
//			//there is an intersection point
//			if (currentTriangle.intersect_line(firstPyramid.corners[rec::pyramid::apex], currentEdgeDirection, hitPoint, hitDistance)) {
//
//				//standard case: add intersection point to the list of all intersection points
//				if (hitDistance > 0) {
//					allIntersectionPoints.push_back(hitPoint);
//					intersectionPointsTrianglesSecondEdgesFirst.push_back(hitPoint);
//				}
//				//intersection point is outside of the frustum (on the "wrong" side of the apex) -> intersection of the frustums is inifinite 
//				else {
//					//calculate intersection point of the ray with the workspace bounding box
//					hitPoint = util::calculateIntersectionPointRayInAABBwithAABB(firstPyramid.corners[rec::pyramid::apex], currentEdgeDirection, workspace);
//					allIntersectionPoints.push_back(hitPoint);
//					intersectionPointsTrianglesSecondEdgesFirst.push_back(hitPoint);
//				}
//			}
//		}
//	}
//
//	//text_output::writeVectorListToTextFile("intersectionPointsTrianglesSecondEdgesFirst.txt", intersectionPointsTrianglesSecondEdgesFirst);
//	//text_output::writeVectorListToTextFile("intersectionPointsTrianglesFirstEdgesSecond.txt", intersectionPointsTrianglesFirstEdgesSecond);
//
//	//text_output::writeVectorListToTextFile("allIntersectionPoints.txt", allIntersectionPoints);
//
//
//rec:aabb intersectionBoundingBox = util::createBoundingBoxOfPoints(allIntersectionPoints);
//
//	//intersect bounding box with the workspace bounding box
//	if (util::doAABBsIntersect(intersectionBoundingBox, workspace)) {
//		result = util::calculateAABBIntersection(intersectionBoundingBox, workspace);
//		return true;
//	}
//	return false;
//}



//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




//###########################################################################################################################################
//###########################################################################################################################################
//###########################################################################################################################################




