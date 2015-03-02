#ifndef SE_SPLIT_H
#define SE_SPLIT_H

#include <iostream>
#include <string>


void splitImage(std::string inputFile, double xc, double yc, double theta, std::string outputFile1, std::string outputFile2);

void getContours(std::string inputFile, std::string contourFile);


#endif /* SE_SPLIT_H*/
