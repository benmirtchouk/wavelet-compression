#ifndef H_WAVELET
#define H_WAVELET

#include <matrix.h>

void dwt(const Span& x, Span& y, int n);
void idwt(const Span& y, Span& x, int n);
void dwt2(const Matrix& x, Matrix& y, int n);
void idwt2(const Matrix& y, Matrix& x, int n);

#endif