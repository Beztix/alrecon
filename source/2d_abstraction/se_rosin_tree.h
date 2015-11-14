#ifndef SE_ROSIN_TREE_H
#define SE_ROSIN_TREE_H


#include "tree.hh"
#include "se_util.h"
#include "rec_seAndFrust.h"

namespace se {


	void startRosinTree(int cam, tree<se::contourAndSe> &seTree, tree<rec::seAndFrust> &seAndFrustTree, std::vector<std::vector<cv::Point>> contours, int width, int height, std::vector<int> qualityValues);

	std::vector<se::superellipse> getEllipsesOfGivenDepth(tree<se::contourAndSe> seTree, int depth);
}


#endif /* SE_ROSIN_TREE_H*/