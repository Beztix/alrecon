#ifndef SE_RENDERING_H
#define SE_RENDERING_H

#include <iostream>
#include <vector>


int processSuperellipsesFromTextfile(std::string input, std::string output);
int processSuperellipsesFromTextfileCeres(std::string input, std::string output);


int processSuperellipsesFromArray(double* ellipses, std::string outputFile);

int processSuperellipsesFromVector(std::vector<std::vector<double>> superellipses, std::string outputFile);


#endif /* SE_RENDERING*/
