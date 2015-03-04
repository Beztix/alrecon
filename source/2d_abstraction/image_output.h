#ifndef IMAGE_OUTPUT_H
#define IMAGE_OUTPUT_H

#include <iostream>
#include <vector>
#include <tuple>


namespace image_output {

	void renderArrayOfLinesToImage(std::string name, int width, int height, int num, double data[1000]);

	void pixelVectorToImage(std::vector<int> pixels, int width, int height, std::string imageName);

	void writeSuperellipsesToImage(std::vector<std::vector<std::tuple<double, double>>> pointListContainer, std::string imgName, int width, int height);
}


#endif /* IMAGE_OUTPUT_H*/
