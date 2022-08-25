#include <compress.h>
#include <wavelet.h>
#include <matrix.h>
#include <quantization.h>
#include <huffman.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>

using std::vector;
using std::string;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::fixed;
using std::setprecision;

void write(const vector<bool>& data, ofstream& file) {
  assert(data.size() % 8 == 0);

  for (int i = 0; i < data.size(); i += 8) {
    BYTE x = 0;
    for (int b = 0; b < 8; b++) {
      x |= data[i + b] << b;
    }
    file.write(reinterpret_cast<char*>(&x), sizeof(BYTE));
  }
}

void compress(const Matrix& image, int n, int q, const string& outputFilename) {
  Matrix output(n, n);
  dwt2(image, output, n);

  vector<int> d = get_values(output, n);
  const auto& [b, y] = lloyd_quantize(d, q, 0.01);
  vector<BYTE> quantized = quantize(output, n, b);
  assert(y.size() == q);

  vector<bool> encoded = huffmanEncode(quantized);

  ofstream file(outputFilename, ios::binary);
  file.write(reinterpret_cast<char*>(&n), sizeof(int));
  file.write(reinterpret_cast<char*>(&q), sizeof(int));
  for (int x : y) {
    file.write(reinterpret_cast<char*>(&x), sizeof(int));
  }
  write(encoded, file);
}

Matrix readImage(BMP& image) {
  if (image.TellWidth() != image.TellHeight()) {
    cout << "Error: expected a square image.\n";
    return Matrix(0, 0);
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
    cout << "Warning: expected a grayscale input; only the red channel will be compressed.\n";
  }

  Matrix data(image);
  return data;
}

int compress(const string& inputFilename, int q, const string& outputFilename) {
  BMP image;
  if (!image.ReadFromFile(inputFilename.c_str())) {
    return 2;
  }

  cout << "Image: " << image.TellWidth() << " x " << image.TellHeight() << "\n";
  cout << "Compressing with quantization: " << q << "\n";
  if (q <= 0 || q >= 256) {
    cout << "Error: quantization must be between 1 and 255.\n";
    return 1;
  }

  int n = image.TellHeight();

  Matrix data = readImage(image);
  if (data.height() == 0) return 1;

  compress(image, n, q, outputFilename);
  return 0;
}

int uncompress(const string& inputFilename, const string& outputFilename) {
  ifstream file(inputFilename, ios::binary);
  file.unsetf(ios::skipws);

  int n, q;
  file.read((char*)&n, sizeof(int));
  file.read((char*)&q, sizeof(int));

  vector<int> y(q);
  for (int i = 0; i < q; i++) {
    file.read((char*)&y[i], sizeof(int));
  }

  int headerSize = sizeof(int) * (2 + q), fileSize;
  file.seekg(0, ios::end);
  fileSize = (int)file.tellg() - headerSize;
  file.seekg(headerSize, ios::beg);

  cout << "Read image size: " << n << " x " << n << "\n";
  cout << "Quantized with value: " << q << "\n";

  vector<BYTE> fileData(fileSize);
  file.read((char*)&fileData[0], fileSize);
  vector<BYTE> data = huffmanDecode(fileData, n * n);

  Matrix output(n, n);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      assert(0 <= data[n * i + j] && data[n * i + j] < q);
      output(i, j) = y[data[n * i + j]];
    }

  Matrix uncompressed(n, n);
  idwt2(output, uncompressed, n);

  BMP image;
  image.SetSize(n, n);
  image.SetBitDepth(24);
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
      double val = uncompressed(i, j);
      BYTE color = std::min(255, std::max(0, (int)val));
      image.SetPixel(j, i, RGBApixel{ color, color, color, 0 });
    }
  image.WriteToFile(outputFilename.c_str());

  return 0;
}

int compare(const std::string& filename1, const std::string& filename2) {
  BMP image1, image2;
  if (!image1.ReadFromFile(filename1.c_str()) || !image2.ReadFromFile(filename2.c_str())) {
    return 2;
  }

  Matrix data1 = readImage(image1), data2 = readImage(image2);
  if (data1.height() == 0 || data2.height() == 0) return 1;

  int n = data1.height();
  if (data2.height() != n) {
    cout << "Error: The two images must be the same size\n";
    return 1;
  }

  double mse = 0, mseInner = 0;
  int innerArea = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      double e2 = (data1(i, j) - data2(i, j)) * (data1(i, j) - data2(i, j));
      mse += e2;

      if (5 < i && i < n - 5 && 5 < j && j < n - 5) {
        mseInner += e2;
        innerArea++;
      }
    }
  }
  mse /= n * n;
  mseInner /= innerArea;

  cout << fixed << setprecision(3);
  cout << "MSE: " << mse << "\n";
  cout << "MSE Inner: " << mseInner << "\n";

  return 0;
}