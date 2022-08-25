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

  int height() const { return d_data.size(); }
  int width() const { return height() == 0 ? 0 : d_data[0].size(); }

  Span getRow(int index);
  const Span getRow(int index) const;
  Span getCol(int index);
  const Span getCol(int index) const;

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