#include <matrix.h>
#include <algorithm>
#include <cassert>

Matrix::Matrix(int n, int m) : d_data(n, std::vector<double>(m)) {
}

Matrix::Matrix(BMP& bmp) : d_data(bmp.TellHeight(), std::vector<double>(bmp.TellWidth())) {
  for (int i = 0; i < bmp.TellHeight(); i++)
    for (int j = 0; j < bmp.TellWidth(); j++)
      d_data[i][j] = bmp(j, i)->Red;
}

double& Matrix::operator()(int i, int j) {
  if (i < 0 || i >= d_data.size() || j < 0 || j >= d_data[i].size()) assert(false);
  return d_data[i][j];
}
const double Matrix::operator()(int i, int j) const {
  if (i < 0 || i >= d_data.size() || j < 0 || j >= d_data[i].size()) assert(false);
  return d_data[i][j];
}

Span Matrix::getRow(int i) { return Span(this, i, true); }
const Span Matrix::getRow(int i) const { return Span(const_cast<Matrix*>(this), i, true); }
Span Matrix::getCol(int i) { return Span(this, i, false); }
const Span Matrix::getCol(int i) const { return Span(const_cast<Matrix*>(this), i, false); }

Span::Span(Matrix* matrix, int index, bool isRow)
  : d_matrix(matrix), d_index(index), d_isRow(isRow)
  , d_offset(0), d_upsample(1) {}
Span::Span(Matrix* matrix, int index, bool isRow, int offset, int upsample)
  : d_matrix(matrix), d_index(index), d_isRow(isRow)
  , d_offset(offset), d_upsample(upsample) {}

double& Span::operator()(int i) {
  if (i % d_upsample != 0) assert(false);
  i = i / d_upsample + d_offset;

  if (d_isRow) return (*d_matrix)(d_index, i);
  return (*d_matrix)(i, d_index);
}
const double Span::operator()(int i) const {
  if (i % d_upsample != 0) return 0;
  i = i / d_upsample + d_offset;

  if (d_isRow) return (*d_matrix)(d_index, i);
  return (*d_matrix)(i, d_index);
}

Span Span::subspan(int offset, int upsample) {
  return Span(d_matrix, d_index, d_isRow, offset, upsample);
}