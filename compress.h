#ifndef H_COMPRESS
#define H_COMPRESS

#include <matrix.h>

#include <string>

int compress(const std::string& inputFilename, int q, const std::string& outputFilename);
int uncompress(const std::string& inputFilename, const std::string& outputFilename);
int compare(const std::string& filename1, const std::string& filename2);

#endif