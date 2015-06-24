#ifndef SE_ROSIN_TREE_H
#define SE_ROSIN_TREE_H


#include "tree.hh"

namespace se {


	void startRosinTree(tree<se::contourAndSe> &seTree, std::vector<std::vector<cv::Point>> contours, int width, int height, std::vector<int> qualityValues);

	std::vector<se::superellipse> getEllipsesOfGivenDepth(tree<se::contourAndSe> seTree, int depth);
}


#endif /* SE_ROSIN_TREE_H*/