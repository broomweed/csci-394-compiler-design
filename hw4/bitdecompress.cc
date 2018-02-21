/*
 * Example "compression" program to encode an input file using Huffman
 * encoding. The output is currently just a string of zeros and ones,
 * not actual bits.
 * The input comes in through stdandard input, and the output goes to
 * standard output. Use shell redicrection to compress files.
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <cassert>

#include "huffman.hh"

using namespace std;
using namespace huffman;

Huffman::encoding_t string_to_bits(std::string str) {
    Huffman::encoding_t bits;
    for (auto c : str) {
        for (unsigned i = 0; i < 8; i++) {
            if (c & (1 << i)) {
                bits.push_back(huffman::Huffman::ONE);
            } else {
                bits.push_back(huffman::Huffman::ZERO);
            }
        }
    }
    return bits;
}

int main(int, char**)
{
  huffman::Huffman huff;

  // Assuming input is a single line, read it all into a string:
  string line;
  getline(cin, line);

  // And convert it to bit_t:
  Huffman::encoding_t input = string_to_bits(line);
  auto b = input.cbegin();
  auto e = input.cend();

  // Iterate over input bits, output their decoding
  // and update their frequency:
  while (b != e) {
      const auto symbol = huff.decode(b, e);
      assert(b <= e);
      if (!symbol && b==e) {
          break;
      } else {
          cout << symbol;
          huff.incFreq(symbol);
      }
  }

  return 0;
}

