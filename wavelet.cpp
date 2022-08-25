#include <wavelet.h>

#include <vector>
#include <cassert>

using std::vector;

// DAUBECHIES2
const vector<double> DB2_LPF_D = { -0.12940952255092145, 0.22414386804185735, 0.836516303737469, 0.48296291314469025 };
const vector<double> DB2_HPF_D = { -0.48296291314469025, 0.836516303737469, -0.22414386804185735, -0.12940952255092145 };
const vector<double> DB2_LPF_R = { 0.48296291314469025, 0.836516303737469, 0.22414386804185735, -0.12940952255092145 };
const vector<double> DB2_HPF_R = { -0.12940952255092145, -0.22414386804185735, 0.836516303737469, -0.48296291314469025 };

// DAUBECHIES1
const vector<double> DB1_LPF_D = { 0.7071067811865476, 0.7071067811865476 };
const vector<double> DB1_HPF_D = { -0.7071067811865476, 0.7071067811865476 };
const vector<double> DB1_LPF_R = { 0.7071067811865476, 0.7071067811865476 };
const vector<double> DB1_HPF_R = { 0.7071067811865476, -0.7071067811865476 };

vector<double> convolve(const Span& x, const vector<double>& h, int n) {
  int m = h.size();
  assert(m <= n);

  vector<double> y(n);
  for (int i = 0; i < n; i++) {
    for (int k = 0; k < m; k++) {
      int idx = i - k + m / 2;
      if (idx < 0) idx = -(idx + 1);
      else if (idx >= n) idx = n - (idx - n + 1);
      y[i] += x(idx) * h[k];
    }
  }

  return y;
}

// Discrete wavelet transform (1-D)
void dwt(const Span& x, Span& y, int n) {
  assert(n % 2 == 0);
  vector<double> lo = convolve(x, DB1_LPF_D, n);
  vector<double> hi = convolve(x, DB1_HPF_D, n);

  for (int i = 0; i < n / 2; i++) {
    y(i) = lo[i * 2];
    y(i + n / 2) = hi[i * 2];
  }
}

// Inverse discrete wavelet transform (1-D)
void idwt(const Span& y, Span& x, int n) {
  assert(n % 2 == 0);

  const Span yL = const_cast<Span&>(y).subspan(0, 2);
  const Span yH = const_cast<Span&>(y).subspan(n / 2, 2);

  vector<double> lo = convolve(yL, DB1_LPF_R, n);
  vector<double> hi = convolve(yH, DB1_HPF_R, n);

  for (int i = 0; i < n; i++) {
    x((i + n + 1) % n) = lo[i] + hi[i];
  }
}

// Discrete wavelet transform (2-D)
void dwt2(const Matrix& x, Matrix& y, int n) {
  Matrix tmp(n, n);

  for (int i = 0; i < n; i++) {
    const Span x_row = x.getRow(i);
    Span tmp_row = tmp.getRow(i);
    dwt(x_row, tmp_row, n);
  }

  for (int i = 0; i < n; i++) {
    const Span tmp_col = tmp.getCol(i);
    Span y_col = y.getCol(i);
    dwt(tmp_col, y_col, n);
  }
}

// Inverse discrete wavelet transform (2-D)
void idwt2(const Matrix& y, Matrix& x, int n) {
  Matrix tmp(n, n);

  for (int i = 0; i < n; i++) {
    Span tmp_col = tmp.getCol(i);
    const Span y_col = y.getCol(i);
    idwt(y_col, tmp_col, n);
  }

  for (int i = 0; i < n; i++) {
    Span x_row = x.getRow(i);
    const Span tmp_row = tmp.getRow(i);
    idwt(tmp_row, x_row, n);
  }
}