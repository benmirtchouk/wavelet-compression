#include <compress.h>

#include <iostream>

using std::cout;
using std::string;
using std::stoi;

int printUsage(char* cmd) {
  cout << "Usage: " << cmd << " compress [input file] [quantization] [output file]\n";
  cout << "Usage: " << cmd << " uncompress [input file] [output file]\n";
  cout << "Usage: " << cmd << " compare [file 1] [file 2]\n";
  return 1;
}

int main(int argc, char* argv[]) {
  if (argc < 2) return printUsage(argv[0]);

  if (string(argv[1]) == "compress") {
    if (argc != 5) return printUsage(argv[0]);
    return compress(argv[2], stoi(argv[3]), argv[4]);
  } else if (string(argv[1]) == "uncompress") {
    if (argc != 4) return printUsage(argv[0]);
    return uncompress(argv[2], argv[3]);
  }  else if (string(argv[1]) == "compare") {
    if (argc != 4) return printUsage(argv[0]);
    return compare(argv[2], argv[3]);
  } else return printUsage(argv[0]);
}