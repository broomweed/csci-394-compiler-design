/*
 * Example "compression" program to encode an input file using Huffman
 * encoding. The output is currently just a string of zeros and ones,
 * not actual bits.
 * The input comes in through stdandard input, and the output goes to
 * standard output. Use shell redicrection to compress files.
 */

#include <iostream>
#include <fstream>
#include <string>

#include "huffman.hh"

using namespace std;

void addnewbit(std::vector<char>& vec, unsigned index, huffman::Huffman::bit_t bit) {
    unsigned byte_index = index / 8;
    unsigned bit_index = index % 8;
    while (byte_index >= vec.size()) {
        vec.push_back(0);
    }
    vec[byte_index] |= (bit << bit_index);
}

int main(int argc, char** argv)
{
  const bool verbose = argc > 1 && string(argv[1]) == "-v";
  huffman::Huffman huff;

  std::vector<char> encoded;
  unsigned bitindex = 0;

  // Read in all of stdin, line by line.
  // Iterate over input characters, output their encoding
  // and update their frequency:
  for (string line; getline(cin, line); ) {
      for (auto c : line + '\n') {
          if (verbose)  cout << c << "\t";
          for (auto bit : huff.encode(c)) {
              addnewbit(encoded, bitindex, bit);
              bitindex ++;
          }
          huff.incFreq(c);
          if (verbose) cout << "\n";
      }
  }

  // Finally, output end-of-file code
  if (verbose) cout << "EOF\t";
  for (auto bit : huff.eofCode()) {
      addnewbit(encoded, bitindex, bit);
      bitindex ++;
  }
  if (verbose) cout << "\n";

  for (auto c : encoded) {
      printf("%c", c);
  }
  printf("\n");

  return 0;
}

