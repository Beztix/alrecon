/**
*************************************************************************
*
* @file as_cgal.cpp
*
* Class for using CGAL to calculate alpha shapes, implementation.
*
************************************************************************/

#include "as_cgal.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/algorithm.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include "Alpha_shape_2.h"
#include "image_input.h"
#include "util.h"


using namespace cv;
using namespace std;



typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::FT FT;
typedef K::Point_2  Point_2;
typedef K::Segment_2  Segment;
typedef CGAL::Alpha_shape_vertex_base_2<K> Vb;
typedef CGAL::Alpha_shape_face_base_2<K>  Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Triangulation_2;
typedef CGAL::Alpha_shape_2<Triangulation_2>  Alpha_shape_2;
typedef Alpha_shape_2::Alpha_shape_edges_iterator Alpha_shape_edges_iterator;




template <class OutputIterator>
void
alpha_edges(const Alpha_shape_2&  A,
OutputIterator out)
{
	for (Alpha_shape_edges_iterator it = A.alpha_shape_edges_begin();
		it != A.alpha_shape_edges_end();
		++it){
		*out++ = A.segment(*it);
	}
}





///converts set pixels represented by an integer array to a list of Point_2

list<Point_2> convertPixelsToPoints(vector<int> pixels, int width, int height) {
	list<Point_2> points;

	//create corresponding points to set pixels
	for (int i = 0; i < height*width; i++) {
		if (pixels[i] != 0){
			int y = i / width;
			int x = i % width;

			Point_2 p = Point_2(x, y);
			points.emplace_back(p);
		}
	}

	return points;
}










///creates an image out of a given vector of segments
/**
 * This method creates an image of the given width and height at the given location/name and writes the given segments into it.
 * End points of the segments are drawn in red, the inner parts of the segments are drawn in light blue (background is black).
 */

void writeSegmentsToImage(string imageLocation, std::vector<Segment> segments, int width, int height) {

	Mat alphaImage = Mat(height, width, CV_8UC3);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			alphaImage.at<Vec3b>(Point(x, y)) = Vec3b(0, 0, 0);
		}
	}

	Point_2 p1;
	Point_2 p2;
	Point point1;
	Point point2;
	Segment seg;
	Vec3b color;

	// for each segment mark its end points in the image
	for (std::vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it) {
		seg = *it;
		p1 = seg.source();
		p2 = seg.target();
		point1 = Point(int(p1.x()), int(p1.y()));
		point2 = Point(int(p2.x()), int(p2.y()));

		//draw the line segment (light blue)
		line(alphaImage, point1, point2, Scalar(255, 200, 20), 1, 8, 0);

		//draw the end points of the segment (red)
		alphaImage.at<Vec3b>(point1) = Vec3b(0, 50, 255);
		alphaImage.at<Vec3b>(point2) = Vec3b(0, 50, 255);


	}

	imwrite(imageLocation, alphaImage);
}






// Calls the utility methods to load points from an image, calculates the alpha shape with a given parameter and save the calculated shapes to an image
/**
 * This method performs all steps needed to evaluate a binary image using alpha shapes:
 * The binary input image is first loaded using OpenCV, the interior pixels of the contained shapes are then optionally reduced 
 * to reduce computational load by alpha shape computation. 
 * The alpha shape is afterwards computed using CGAL and the computed segments are written to a new image using OpenCV.
 * The time needed for each step is evaluated and printed on console.
*/

int useCgal(string inputImage, string reducedImgName, string outputImage, int reduction)
{
	int width;
	int height;
	auto startTime = chrono::high_resolution_clock::now();

	//loading binary input image
	int* pixels = image_input::loadPixelsFromImage(inputImage, width, height);
	if (pixels == nullptr){
		return -1;
	}
	auto loadTime = chrono::high_resolution_clock::now();


	//reducing pixels
	vector<int> reducedPixels = util::reducePixels(pixels, width, height, reduction, reducedImgName);
	auto reduceTime = chrono::high_resolution_clock::now();


	//converting pixels
	list<Point_2> points = convertPixelsToPoints(reducedPixels, width, height);
	auto convertTime = chrono::high_resolution_clock::now();

	std::cout << "Image dimensions: " << width << " x " << height << std::endl;
	std::cout << "list size:" << points.size() << std::endl;
	

	//computing alpha shape
	Alpha_shape_2 A(points.begin(), points.end(), FT(100), Alpha_shape_2::GENERAL);
	std::vector<Segment> segments;
	alpha_edges(A, std::back_inserter(segments));
	std::cout << "Alpha Shape computed: there are "<< segments.size() << " alpha shape edges" << std::endl;
	auto alphaTime = chrono::high_resolution_clock::now();


	//writing alpha shape image
	writeSegmentsToImage(outputImage, segments, width, height);
	auto writeImageTime = chrono::high_resolution_clock::now();




	//evaluation time elapsed
	auto loadDuration = loadTime - startTime;
	auto reduceDuration = reduceTime - loadTime;
	auto convertDuration = convertTime - reduceTime;
	auto alphaDuration = alphaTime - convertTime;
	auto writeImageDuration = writeImageTime - alphaTime;

	cout << "loadDuration:     " << chrono::duration_cast<chrono::microseconds>(loadDuration).count() << endl;
	cout << "reduceDuration:   " << chrono::duration_cast<chrono::microseconds>(reduceDuration).count() << endl;
	cout << "convertDuration:  " << chrono::duration_cast<chrono::microseconds>(convertDuration).count() << endl;
	cout << "alphaDuration:    " << chrono::duration_cast<chrono::microseconds>(alphaDuration).count() << endl;
	cout << "writeImageDuration:    " << chrono::duration_cast<chrono::microseconds>(writeImageDuration).count() << endl;

	return 0;
}