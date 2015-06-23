

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "se_util.h"
#include "se_split.h"
#include "tree.hh"
#include "se_rosin_core.h"


using namespace std;


namespace se {


	vector<se::contourAndSe> processPartForRecursiveRosinForTree(se::contourAndSe currentFit, int requestedQualityValue, vector<cv::Point> part);




	se::superellipse useRosinOnce(vector<cv::Point> contourPoints, const int width, const int height) {

		//fit one ellipse to the contour
		vector<int> contourVector;
		for (int j = 0; j < contourPoints.size(); j++) {
			contourVector.push_back(contourPoints.at(j).x);
			contourVector.push_back(contourPoints.at(j).y);
		}
		int no_contourPixels = int(contourVector.size()) / 2;
		int* contourList = &contourVector[0];

		double xc, yc, theta, a, b, epsilon;
		fitEllipseRosin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);

		//make "a" the larger axis of the fitted superellipse
		if (a < b) {
			double temp = a;
			a = b;
			b = temp;
			theta += 1.57079;
		}

		//evaluate the fit
		double currentQualityValue = se::evaluateFitEuclidean(contourPoints, xc, yc, theta, a, b, epsilon);


		se::superellipse initialFit = se::superellipse(xc, yc, theta, a, b, epsilon, currentQualityValue);

		return initialFit;
	}









	vector<se::contourAndSe> recursiveRosinForTree(se::contourAndSe currentFit, int requestedQualityValue) {

		//===========================================
		//==    do the fit for the whole contour   ==
		//===========================================
		double currentQualityValue;
		double xc, yc, theta, a, b, epsilon;

		//========   fit for given contour is not calculated yet   =======
		if (currentFit.alreadyFitted == false) {

			//fit one ellipse to the contour
			vector<int> contourVector;
			for (int j = 0; j < currentFit.contour.size(); j++) {
				contourVector.push_back(currentFit.contour.at(j).x);
				contourVector.push_back(currentFit.contour.at(j).y);
			}
			int no_contourPixels = int(contourVector.size()) / 2;
			int* contourList = &contourVector[0];

			fitEllipseRosin2(contourList, no_contourPixels, &xc, &yc, &theta, &a, &b, &epsilon);

			#ifdef DEBUG
			image_output::pixelVectorToImage(contourVector, width, height, "initialContour.png");
			#endif

			//make "a" the larger axis of the fitted superellipse
			if (a < b) {
				double temp = a;
				a = b;
				b = temp;
				theta += 1.57079;
			}

			#ifdef DEBUG
			image_output::renderSuperEllipseToImage("initialEllipse.png", width, height, xc, yc, theta, a, b, epsilon);
			#endif

			//evaluate the fit
			currentQualityValue = se::evaluateFitEuclidean(currentFit.contour, xc, yc, theta, a, b, epsilon);
		}

		//========   fit for given contour is already calculated   =======
		else {
			currentQualityValue = currentFit.fittedEllipse.quality;
			xc = currentFit.fittedEllipse.xc;
			yc = currentFit.fittedEllipse.yc;
			theta = currentFit.fittedEllipse.theta;
			a = currentFit.fittedEllipse.a;
			b = currentFit.fittedEllipse.b;
			epsilon = currentFit.fittedEllipse.epsilon;
		}


		//=====================================
		//==    fit is already good enough   ==
		//=====================================
		if (currentQualityValue < requestedQualityValue) {
			//test if the fit is already conservative
			bool conservative = se::isFitConservative(currentFit.contour, xc, yc, theta, a, b, epsilon);

			//if not conservative: enlarge the superellipse until it is conservative
			while (!conservative) {
				a += 0.5;
				b += 0.5;
				if (epsilon > 2) {
					epsilon -= 0.3;
				}
				conservative = se::isFitConservative(currentFit.contour, xc, yc, theta, a, b, epsilon);
			}

			//return the fitted ellipse
			se::superellipse conservativeEllipse = se::superellipse(xc, yc, theta, a ,b ,epsilon, currentQualityValue);
			currentFit.setEllipse(conservativeEllipse);

			vector<se::contourAndSe> resultVector;
			resultVector.emplace_back(currentFit);
			return std::move(resultVector);
		}


		//===========================================================
		//==    fit is not good enough yet: proceed recursively:   ==
		//===========================================================
		else {

			//split the contour
			vector<cv::Point> part1;
			vector<cv::Point> part2;
			splitContourToVectors(currentFit.contour, currentFit.width, currentFit.height, xc, yc, theta, part1, part2);

			#ifdef DEBUG
			image_output::renderContourColored("originalContour.png", width, height, contourPoints);
			image_output::renderContourColored("contourPart1.png", width, height, part1);
			image_output::renderContourColored("contourPart2.png", width, height, part2);
			#endif

			vector<se::contourAndSe> resultVector1 = processPartForRecursiveRosinForTree(currentFit, requestedQualityValue, part1);
			vector<se::contourAndSe> resultVector2 = processPartForRecursiveRosinForTree(currentFit, requestedQualityValue, part2);

			//combine the fitted ellipses from the recursive calls and return them
			resultVector1.insert(resultVector1.end(), resultVector2.begin(), resultVector2.end());
			return std::move(resultVector1);
		}


	}






	vector<se::contourAndSe> processPartForRecursiveRosinForTree(se::contourAndSe currentFit, int requestedQualityValue, vector<cv::Point> part) {
		vector<se::contourAndSe> totalResultVector;

		cv::Mat partMat = cv::Mat(currentFit.height, currentFit.width, CV_8UC1);
		partMat.setTo(0);
		for (int i = 0; i < part.size(); i++) {
			//add the points of part1 to the Mat
			cv::Point currentPoint = part.at(i);
			int x = currentPoint.x - 0;
			int y = currentPoint.y - 0;
			partMat.at<uchar>(y, x) = 1;
		}

		//find the contours in part
		vector<vector<cv::Point>> contours;
		cv::findContours(partMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

		//for each contour (equals blob) in part1
		for (int i = 0; i < contours.size(); i++) {
			vector<cv::Point> currentContour = contours.at(i);
			//image_output::renderContourColored("part1_" + to_string(i) + ".png", width, height, currentContour);


			//threshold: if contour is too small don't fit to the contour (just ignore)
			if (currentContour.size() < 5) {
				//TODO: HOW TO DEAL WITH THIS??
				continue;
			}

			//determine dimension and offset of the contour
			int minX = INT_MAX;
			int maxX = INT_MIN;
			int minY = INT_MAX;
			int maxY = INT_MIN;
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				if (currentPoint.x < minX) minX = currentPoint.x;
				if (currentPoint.x > maxX) maxX = currentPoint.x;
				if (currentPoint.y < minY) minY = currentPoint.y;
				if (currentPoint.y > maxY) maxY = currentPoint.y;
			}
			int sizeX = maxX - minX + 3;
			int sizeY = maxY - minY + 3;
			int newOffsetX = minX - 1;
			int newOffsetY = minY - 1;

			//substract the offset from the contour points
			vector<cv::Point> currentContourWithOffset(currentContour.size());
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				cv::Point newPoint = cv::Point(currentPoint.x - newOffsetX, currentPoint.y - newOffsetY);
				currentContourWithOffset.at(j) = newPoint;
			}
			//image_output::renderContourColored("part1_" + to_string(i) + "_offset.png", sizeX, sizeY, currentContourWithOffset);


			//calculate new total offset
			int totalOffsetX = currentFit.offSetX + newOffsetX;
			int totalOffsetY = currentFit.offSetY + newOffsetY;


			//create a new contourAndSe for the contour
			se::contourAndSe casOfPart = se::contourAndSe(currentContourWithOffset, sizeX, sizeY, totalOffsetX, totalOffsetY);

			//use the method recursively on the new container
			vector<se::contourAndSe> resultVector = recursiveRosinForTree(casOfPart, requestedQualityValue);

			totalResultVector.insert(totalResultVector.end(), resultVector.begin(), resultVector.end());
		}

		return totalResultVector;

	}












	void expandNodeToNextQualityLevel(tree<se::contourAndSe> seTree, tree<se::contourAndSe>::iterator nodePointer, int requestedQualityValue) {
		se::contourAndSe initialNode = *nodePointer;



		if (!initialNode.alreadyFitted) {
			se::superellipse fit = useRosinOnce(initialNode.contour, initialNode.width, initialNode.height);
			initialNode.setEllipse(fit);
		}


		//fit is already good enough, add same ellipse as child
		if (initialNode.fittedEllipse.quality < requestedQualityValue) {
			seTree.append_child(nodePointer, initialNode);
		}

		//improve the fit, add all superellipses as children
		else {
			vector<se::contourAndSe> newChildren = recursiveRosinForTree(initialNode, requestedQualityValue);
			for (int i = 0; i < newChildren.size(); i++) {
				se::contourAndSe newChild = newChildren.at(i);
				seTree.append_child(nodePointer, newChild);
			}
		}

	}








	void startRosinTree(tree<contourAndSe> seTree, vector<vector<cv::Point>> contours, int width, int height, vector<int> qualityValues) {

		tree<se::contourAndSe>::iterator top = seTree.begin();

		//========================================
		//==    initial fit for each contour    ==
		//========================================

		//for each contour:
		for (int i = 0; i < contours.size(); i++) {

			vector<cv::Point> currentContour = contours.at(i);

			//determine dimension and offset of the contour
			int minX = INT_MAX;
			int maxX = INT_MIN;
			int minY = INT_MAX;
			int maxY = INT_MIN;
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				if (currentPoint.x < minX) minX = currentPoint.x;
				if (currentPoint.x > maxX) maxX = currentPoint.x;
				if (currentPoint.y < minY) minY = currentPoint.y;
				if (currentPoint.y > maxY) maxY = currentPoint.y;
			}
			int sizeX = maxX - minX + 3;
			int sizeY = maxY - minY + 3;
			int offsetX = minX - 1;
			int offsetY = minY - 1;


			//substract the offset from the contour points
			vector<cv::Point> currentContourWithOffset(currentContour.size());
			for (int j = 0; j < currentContour.size(); j++) {
				cv::Point currentPoint = currentContour.at(j);
				cv::Point newPoint = cv::Point(currentPoint.x - offsetX, currentPoint.y - offsetY);
				currentContourWithOffset.at(j) = newPoint;
			}


			//use Rosin fitting to fit a single superellipse to the contour
			se::superellipse initialFit = useRosinOnce(currentContourWithOffset, sizeX, sizeY);

			//build a container for the fit
			se::contourAndSe initialContainer = se::contourAndSe(currentContourWithOffset, sizeX, sizeY, offsetX, offsetY, initialFit);

			int size1 = seTree.size();
			//add the initial fit to the seTree
			seTree.append_child(top, initialContainer);

			int size2 = seTree.size();
			int dummy = 5;
		}



		//===================================================================================================================
		//==    iteratively split and fit currently lowest seTree level until desired quality for this level is reached    ==
		//==        seTree then contains fits with desired minimum quality according to qualityValues[] at each level      ==
		//===================================================================================================================

		int currentTreeDepth = 1;
		tree<se::contourAndSe>::fixed_depth_iterator depthBeginIterator;


		while (currentTreeDepth - 1 < qualityValues.size()) {
			//get desired quality for the next depth
			int desiredNextQuality = qualityValues.at(currentTreeDepth - 1);

			//get fixed depth iterator for currentTreeDepth
			depthBeginIterator = seTree.begin_fixed(top, currentTreeDepth);

			//iterate through all fits at the current depth
			while (seTree.is_valid(depthBeginIterator)) {
				se::contourAndSe currentContainer = *depthBeginIterator;

				if (!currentContainer.alreadyFitted) {
					se::superellipse fit = useRosinOnce(currentContainer.contour, currentContainer.width, currentContainer.height);
					currentContainer.setEllipse(fit);
				}


				//if the quality of the fit is already good enough for the next depth
				if (currentContainer.fittedEllipse.quality < desiredNextQuality) {
					//TODO: SINNVOLL??
					//duplicate current node
					seTree.append_child(depthBeginIterator, currentContainer);
				}

				//fit is not good enough for the next depth
				else {
					//this fit needs to be splitted and fitted again until the desired quality for the next level is achieved
					vector<se::contourAndSe> newFits = recursiveRosinForTree(currentContainer, desiredNextQuality);

					//append the new ellipses as children to the current node
					for (int i = 0; i < newFits.size(); i++) {
						se::contourAndSe currentNewFit = newFits.at(i);
						seTree.append_child(depthBeginIterator, currentNewFit);
					}
				}

				depthBeginIterator++;
			}

			currentTreeDepth++;
		}


	}




	vector<se::superellipse> getEllipsesOfGivenDepth(tree<se::contourAndSe> seTree, int depth) {
		vector<se::superellipse> ellipsesVector;
		tree<se::contourAndSe>::iterator top = seTree.begin();
		tree<se::contourAndSe>::fixed_depth_iterator depthBeginIterator = seTree.begin_fixed(top, depth);

		while (seTree.is_valid(depthBeginIterator)) {
			se::contourAndSe currentContainer = *depthBeginIterator;
			se::superellipse currentEllipse = currentContainer.fittedEllipse;
			currentEllipse.xc += currentContainer.offSetX;
			currentEllipse.yc += currentContainer.offSetY;
			ellipsesVector.emplace_back(currentEllipse);
			depthBeginIterator++;
		}

		return ellipsesVector;
	}

}