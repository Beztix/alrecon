#ifndef SE_INPUT_H
#define SE_INPUT_H

#include <iostream>


int prepareInputForRonin(std::string inputImage, std::string reducedImgName, std::string outputFile, int reduction);


int prepareInputForCeres(std::string inputImage, std::string reducedImgName, std::string outputFile, int reduction);

int pixFileToImage(std::string input, std::string output);


#endif /* SE_INPUT_H*/
