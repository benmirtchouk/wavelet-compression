#include <quantization.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>

using std::vector;
using std::pair;
using std::min;
using std::max;
using std::cout;
using std::setprecision;

vector<int> get_values(const Matrix& m, int n) {
  vector<int> vals(n * n);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      vals[n * i + j] = m(i, j);
    }

  sort(vals.begin(), vals.end());
  return vals;
}

void compute_boundaries(const vector<int>& y, vector<int>& b) {
  int n = y.size();

  for (int i = 1; i < n; i++) {
    b[i] = (y[i - 1] + y[i]) / 2;
  }
}

void compute_reconstruction_levels(const vector<int>& x, const vector<int>& b, vector<int>& y) {
  int n = y.size(), i = 0;
  long long num = 0;
  int den = 0;

  for (double v : x) {
    assert(v - b[i] >= -0.01);
    while (i + 1 < n && v >= b[i + 1]){
      y[i] = den == 0 ? 0 : num / den;
      num = den = 0;
      i++;
    }

    num += v;
    den++;
  }
}

long long compute_distortion(const vector<int>& x, const vector<int>& b, const vector<int>& y) {
  int n = y.size(), i = 0;
  long long D = 0;

  for (int v : x) {
    assert(v >= b[i]);
    while (v >= b[i + 1]) i++;

    assert(i < n);
    D += 1ll * (v - y[i]) * (v - y[i]);
  }

  return D;
}

// return [b, y] where b are the decision boundaries and y are the (n) reconstruction values
pair<vector<int>, vector<int>> lloyd_quantize(const vector<int>& x, int n, double e) {
  int L = x.size();
  vector<int> b(n + 1);
  vector<int> y(n);
  long long D = -100;

  int mn = x[0], mx = x[0];
  for (int v : x) {
    mn = min(mn, v);
    mx = max(mx, v);
  }

  for (int i = 0; i < n; i++) y[i] = (i + 1) * (mx - mn) / (n + 1) + mn;
  b[0] = x[0];
  b[n] = x[L-1] + 1;

  while (true) {
    compute_boundaries(y, b);
    long long D2 = compute_distortion(x, b, y);

    if (abs(D - D2) < e * x.size()) break;
    D = D2;
    compute_reconstruction_levels(x, b, y);
  }

  return { b, y };
}

vector<BYTE> quantize(const Matrix& m, int n, const vector<int>& b) {
  vector<BYTE> output(n * n);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      auto it = upper_bound(b.begin(), b.end(), (int)m(i, j));
      assert(it != b.end() && it != b.begin());

      int idx = it - b.begin() - 1;
      output[n*i+j] = idx;
    }

  return output;
}