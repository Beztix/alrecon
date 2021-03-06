#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <opencv2/core/core.hpp>
#include <viral_core/geo_vector.hpp>
#include "rec_primitive3D.h"

namespace util {


	struct float3 {
		float x;
		float y;
		float z;

		float3() {
			x = 0;
			y = 0;
			z = 0;
		}

		float3(float mx, float my, float mz) {
			x = mx;
			y = my;
			z = mz;
		}

		float3 operator+(const float3 f) {
			return float3(x + f.x, y + f.y, z + f.z);
		}

		float3 operator-(const float3 f) {
			return float3(x - f.x, y - f.y, z - f.z);
		}

		float3 operator*(const float f) {
			return float3(x*f, y*f, z*f);
		}
	};


	bool allPositiveOrZeroFloat(std::vector<float> floats);

	bool allNegativeOrZeroFloat(std::vector<float> floats);

	void sortPixelFile(std::string input, std::string output);

	std::vector<int> reducePixels(int* pixels, int width, int height, int spacing, std::string reducedImgName);

	std::vector<int> getContourVector(std::vector<int> inputPixelGrid, int width, int height);
	
	int* getFilledPixelGridFromContour(std::vector<cv::Point> contourPoints, int width, int height);

	std::vector<cv::Point> bresenhamLine(const cv::Point p1, const cv::Point p2);

	int testSegmentsIntersection(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);

	cv::Point2d intersectLines(cv::Point a1, cv::Point a2, cv::Point b1, cv::Point b2);


	void getContours(std::string inputFile, std::string contourFile);

	void getContoursRonin(std::string inputFile, std::string contourFile);


	float3 ClosestPointLineLine(float3 start0, float3 end0, float3 start1, float3 end1);

	std::vector<int> createBoundingBoxLinesFromCorners(std::vector<int> cornerPixels);


	rec::aabb createBoundingBoxOfPoints(std::vector<viral_core::vector> points);

	viral_core::vector calculateIntersectionPointRayInAABBwithAABB(viral_core::vector startPoint, viral_core::vector direction, rec::aabb boundingBox);

	bool doAABBsIntersect(rec::aabb bb1, rec::aabb bb2);

	rec::aabb calculateAABBIntersection(rec::aabb bb1, rec::aabb bb2);


	std::vector<viral_core::vector> orderPointsByDistance(viral_core::vector startPoint, std::vector<viral_core::vector> points);


}


#endif /* UTIL_H*/
