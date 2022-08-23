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
  int width = image.TellWidth(), height = image.TellHeight();

  cout << "image: " << width << " x " << height << "\n";

  bool nongrayscale = false;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++) {
      RGBApixel* p = image(i, j);
      if (p->Red != p->Green || p->Red != p->Blue) {
        nongrayscale = true;
      }
    }

  if (nongrayscale) {
    cout << "Warning: Expected a grayscale input; only the red channel will be compressed.\n";
  }


  Matrix data(image);
  Matrix output(height, width);

  dwt2(image, output);

  BMP o = output.toBmp();
  o.WriteToFile("test.bmp");
}