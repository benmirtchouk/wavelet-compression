#include <bits/stdc++.h>
#include <EasyBMP.h>
#include <wavelet.h>
#include <matrix.h>

using namespace std;

void prnt(RGBApixel* p) {
  cout << "{" << (int)p->Red << ", " << (int)p->Green << ", " << (int)p->Blue << ", " << "}";
}

int main() {
  BMP image;
  image.ReadFromFile("../Lenna.bmp");

  cout << "image: " << image.TellWidth() << " x " << image.TellHeight() << "\n";
  if (image.TellWidth() != image.TellHeight()) {
    cout << "Error: Expected a square image.\n";
    return 1;
  }
  int n = image.TellHeight();

  bool nongrayscale = false;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      RGBApixel* p = image(i, j);
      if (p->Red != p->Green || p->Red != p->Blue) {
        nongrayscale = true;
      }
    }
  if (nongrayscale) {
    cout << "Warning: Expected a grayscale input; only the red channel will be compressed.\n";
  }

  Matrix data(image);
  Matrix output(n, n);
  Matrix restored(n, n);
  Matrix error(n, n);

  dwt2(image, output, n);
  idwt2(output, restored, n);

  data.printStats("data");
  // output.printStats("output");
  restored.printStats("restored");

  BMP o = output.toBmp();
  o.WriteToFile("output.bmp");
  BMP r = restored.toBmp(false);
  r.WriteToFile("restored.bmp");

  double MSE = 0, mx = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double e = data(i, j) - restored(i, j);
      mx = max(mx, e);
      MSE += e * e;
      error(i,j) = abs(e);
    }
  }
  cout << "max error: " << mx << "\n";
  cout << "MSE: " << MSE / (n * n) << "\n";

  BMP e = error.toBmp(false);
  e.WriteToFile("error.bmp");
}