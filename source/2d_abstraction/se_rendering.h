#ifndef SE_RENDERING_H
#define SE_RENDERING_H

#include <iostream>
#include <vector>
#include "se_util.h"


int processSuperellipsesFromArray(double* ellipses, std::string outputFile);

int processSuperellipsesFromVector(std::vector<se::superellipse> superellipses, std::string outputFile, const int width, const int height);


#endif /* SE_RENDERING*/
