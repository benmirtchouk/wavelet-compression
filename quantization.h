#ifndef H_QUANTIZATION
#define H_QUANTIZATION

#include <matrix.h>

#include <vector>
#include <utility>

typedef unsigned char BYTE;

std::vector<int> get_values(const Matrix& m, int n);
std::pair<std::vector<int>, std::vector<int>> lloyd_quantize(const std::vector<int>& x, int n, double e);
std::vector<BYTE> quantize(const Matrix& m, int n, const std::vector<int>& b);

#endif