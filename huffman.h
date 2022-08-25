#ifndef H_HUFFMAN
#define H_HUFFMAN

#include <vector>

typedef unsigned char BYTE;

std::vector<bool> huffmanEncode(const std::vector<BYTE>& data);
std::vector<BYTE> huffmanDecode(const std::vector<BYTE>& data, int bytes);
std::vector<BYTE> huffmanDecode(const std::vector<bool>& bits, int bytes);

#endif