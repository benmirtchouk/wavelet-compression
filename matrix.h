#ifndef H_MATRIX
#define H_MATRIX

#include <EasyBMP.h>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

class Span;

class Matrix {
public:
  Matrix(int n, int m);
  Matrix(BMP& bmp);

  double& operator()(int i, int j);
  const double operator()(int i, int j) const;

  Span getRow(int index);
  const Span getRow(int index) const;
  Span getCol(int index);
  const Span getCol(int index) const;

  BMP toBmp(bool scale = true) const;
  void printStats(const std::string& s) {
    int height = d_data.size(), width = d_data[0].size();

    double mn = (*this)(0, 0);
    double mx = mn;
    double avg = 0;

    for (int i = 0; i < height; i++)
      for (int j = 0; j < width; j++) {
        mn = std::min(mn, (*this)(i, j));
        mx = std::max(mx, (*this)(i, j));
        avg += (*this)(i, j);
      }

    avg /= height * width;
    std::cout << s << " stats: " << mn << "|" << avg << "|" << mx << "\n";
    for (int i = 0; i < std::min(10, height); i++) {
      for (int j = 0; j < std::min(10, width); j++)
        std::cout << std::fixed << std::setprecision(1) << std::setw(7) << (*this)(i, j);
      std::cout << "\n";
    }
    std::cout << "\n\n";
  }

private:
  std::vector<std::vector<double>> d_data;
};

class Span {
public:
  Span(Matrix* matrix, int index, bool isRow);
  Span(Matrix* matrix, int index, bool isRow, int offset, int upsample);

  double& operator()(int i);
  const double operator()(int i) const;

  Span subspan(int offset, int upsample);

private:
  Matrix* d_matrix;
  int d_index, d_offset, d_upsample;
  bool d_isRow;
};

#endif