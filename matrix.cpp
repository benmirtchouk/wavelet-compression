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
  if (i < 0 || i >= height() || j < 0 || j >= width()) assert(false);
  return d_data[i][j];
}
const double Matrix::operator()(int i, int j) const {
  if (i < 0 || i >= height() || j < 0 || j >= width()) assert(false);
  return d_data[i][j];
}
int Matrix::height() const { return d_data.size(); }
int Matrix::width() const { return d_data.empty() ? 0 : d_data[0].size(); }

Span Matrix::getRow(int i) { return Span(this, i, true); }
const Span Matrix::getRow(int i) const { return Span(const_cast<Matrix*>(this), i, true); }
Span Matrix::getCol(int i) { return Span(this, i, false); }
const Span Matrix::getCol(int i) const { return Span(const_cast<Matrix*>(this), i, false); }

BMP Matrix::toBmp() const {
  BMP output;
  output.SetSize(width(), height());
  output.SetBitDepth(24);

  double mn = (*this)(0, 0);
  double mx = mn;

  for (int i = 0; i < height(); i++)
    for (int j = 0; j < width(); j++) {
      mn = std::min(mn, (*this)(i,j));
      mx = std::max(mx, (*this)(i,j));
    }

  for (int i = 0; i < height(); i++)
    for (int j = 0; j < width(); j++) {
      double val = ((*this)(i,j) - mn) * 255 / (mx - mn);
      unsigned char color = std::min(255, std::max(0, (int)val));
      output.SetPixel(j, i, RGBApixel{color, color, color, 0});
    }

  return output;
}

Span::Span(Matrix* matrix, int index, bool isRow): d_matrix(matrix), d_index(index), d_isRow(isRow) {}

double& Span::operator()(int i) {
  if (d_isRow) return (*d_matrix)(d_index, i);
  return (*d_matrix)(i, d_index);
}
const double Span::operator()(int i) const {
  if (d_isRow) return (*d_matrix)(d_index, i);
  return (*d_matrix)(i, d_index);
}
int Span::length() const { return d_isRow ? d_matrix->width() : d_matrix->height(); }
