#ifndef H_MATRIX
#define H_MATRIX

#include <EasyBMP.h>
#include <vector>

class Span;

class Matrix {
public:
  Matrix(int n, int m);
  Matrix(BMP& bmp);

  double& operator()(int i, int j);
  const double operator()(int i, int j) const;
  int height() const;
  int width() const;

  Span getRow(int i);
  const Span getRow(int i) const;
  Span getCol(int i);
  const Span getCol(int i) const;

  BMP toBmp() const;

private:
  std::vector<std::vector<double>> d_data;
};

class Span {
public:
  Span(Matrix* matrix, int index, bool isRow);

  double& operator()(int i);
  const double operator()(int i) const;
  int length() const;

private:
  Matrix* d_matrix;
  int d_index;
  bool d_isRow;
};

#endif