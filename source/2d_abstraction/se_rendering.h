#ifndef SE_RENDERING_H
#define SE_RENDERING_H

#include <iostream>
#include <vector>


int processSuperellipsesFromArray(double* ellipses, std::string outputFile);

int processSuperellipsesFromVector(std::vector<std::vector<double>> superellipses, std::string outputFile, const int width, const int height);


#endif /* SE_RENDERING*/
