#include <wavelet.h>
#include <cassert>

using namespace std;

const vector<double> DAUBECHIES2_LPF = {
  -0.12940952255092145,
  0.22414386804185735,
  0.836516303737469,
  0.48296291314469025
};
const vector<double> DAUBECHIES2_HPF = {
  -0.48296291314469025,
  0.836516303737469,
  -0.22414386804185735,
  -0.12940952255092145
};

vector<double> convolve(const Span& x, const vector<double>& h) {
  int n = x.length(), m = h.size();
  assert(m <= n);

  vector<double> y(n);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      y[i] += x((i - j + n) % n) * h[j];
    }
  }

  return y;
}

// Discrete wavelet transform (1-D)
void dwt(const Span& x, Span& y) {
  assert(x.length() % 2 == 0);
  vector<double> lo = convolve(x, DAUBECHIES2_LPF);
  vector<double> hi = convolve(x, DAUBECHIES2_HPF);

  for (int i = 0; i < x.length() / 2; i++) {
    y(i) = lo[i*2];
    y(i + x.length() / 2) = hi[i*2];
  }
}

// Discrete wavelet transform (2-D)
void dwt2(const Matrix& m, Matrix& output) {
  assert(m.height() == output.height() && m.width() == output.width());
  Matrix tmp(m.height(), m.width());

  for (int i = 0; i < m.height(); i++) {
    const Span input_row = m.getRow(i);
    Span output_row = tmp.getRow(i);
    dwt(input_row, output_row);
  }

  for (int i = 0; i < m.width(); i++) {
    const Span input_col = tmp.getCol(i);
    Span output_col = output.getCol(i);
    dwt(input_col, output_col);
  }
}