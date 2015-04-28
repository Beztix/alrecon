#ifndef IMAGE_OUTPUT_H
#define IMAGE_OUTPUT_H

#include <iostream>
#include <vector>
#include <tuple>


namespace image_output {

	void renderArrayOfLinesToImage(std::string name, int width, int height, int num, double data[1000]);

	void pixelVectorToImage(std::vector<int> pixels, int width, int height, std::string imageName);

	void pixelGridToImage(std::vector<int> pixels, int width, int height, std::string imageName);

	void writeSuperellipsesToImage(std::vector<std::vector<std::tuple<double, double>>> pointListContainer, std::string imgName, int width, int height);

	void renderSuperEllipseToImage(std::string imgName, int imgWidth, int imgHeight, double xc, double yc, double theta, double a, double b, double epsilon);

	void combineOriginalAndEllipseImage(std::string originalImageLocation, std::string ellipseImageLocation, std::string outputImageLocation);
}


#endif /* IMAGE_OUTPUT_H*/
