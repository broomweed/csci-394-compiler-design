#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "huffman.hh"

#include <limits.h>
#include <cstdlib>
#include <ctime>

using namespace huffman;

TEST_CASE("All symbols start with different codes", "[diff_codes]") {
    auto huff = Huffman();
    for (unsigned i = 0; i < 256; ++i) {
        for (unsigned j = 0; i < 256; ++i) {
            if (i != j) {
                REQUIRE(huff.encode(i) != huff.encode(j));
            }
        }
    }
}

TEST_CASE("EOF symbol doesn't match any symbol code", "[eof_doesnt_match]") {
    auto huff = Huffman();
    for (unsigned i = 0; i < 256; ++i) {
        REQUIRE(huff.encode(i) != huff.eofCode());
    }
}

TEST_CASE("Most frequent symbol encodes shortest", "[compression]") {
    auto huff = Huffman();
    huff.incFreq(0);
    huff.incFreq(0);
    const auto len0 = huff.encode(0).size();

    for (unsigned i = 1; i < 256; ++i) {
        REQUIRE(huff.encode(i).size() > len0);
    }
}

TEST_CASE("Most frequent symbol always encodes to single bit", "[compression]") {
    auto huff = Huffman();
    huff.incFreq(0);
    huff.incFreq(0);
    REQUIRE(huff.encode(0).size() == 1);
    REQUIRE(huff.encode(1).size() > 1);
    huff.incFreq(1);
    huff.incFreq(1);
    huff.incFreq(1);
    REQUIRE(huff.encode(0).size() > 1);
    REQUIRE(huff.encode(1).size() == 1);
}


TEST_CASE("Encoded strings decode to the same thing, with EOF", "[decompression]") {
    auto huff = Huffman();
    auto huff2 = Huffman();
    std::vector<std::string> teststrs;
    teststrs.push_back("baby beluga in the deep blue sea, swim so wild and you swim so free\n");
    teststrs.push_back("cool cool cool cool coolcool cool cool");
    teststrs.push_back("sadoifjasiodljfcirj98m4oau3rxoj,dlqxwximejdia");
    teststrs.push_back("‘İki ördek, ekmeğim aldı…♯♭ĸə£łß’"); // unicode works, nice
    teststrs.push_back("\t\r\f\v\n\n\r\v\f\\\"\'\a\u23f2\U1293ac2c");
    for (unsigned i = 0; i < teststrs.size(); ++i) {
        // Generate a random string of length i, encode it, then decode it
        Huffman::encoding_t enc;
        std::string to_encode = teststrs[i];
        for (auto c : to_encode) {
            for (auto bit : huff.encode(c)) {
                enc.push_back(bit);
            }
            huff.incFreq(c);
        }
        for (auto bit : huff.eofCode()) {
            enc.push_back(bit);
        }

        // DECODE
        auto b = enc.cbegin();
        auto e = enc.cend();

        std::string dec;
        while (b != e) {
            const auto symbol = huff2.decode(b, e);
            assert(b <= e);
            if (!symbol && b == e) {
                break;
            }
            dec.push_back(symbol);
            huff2.incFreq(symbol);
        }

        REQUIRE(dec == to_encode);
    }
}


TEST_CASE("Random strings decode to the same thing", "[stochastic-decompression]") {
    /* Generate 100 random strings, and encode them, then decode them to
     * make sure they match. */
    srand(time(NULL));
    auto huff = Huffman();
    auto huff2 = Huffman();
    for (unsigned i = 0; i < 100; ++i) {
        // Generate a random string of length i, encode it, then decode it
        std::vector<Huffman::symbol_t> vec;
        for (unsigned int j = 0; j < i; ++j) {
            vec.push_back(rand() % 256);
        }
        // ENCODE
        Huffman::encoding_t enc;
        for (auto c : vec) {
            for (auto bit : huff.encode(c)) {
                enc.push_back(bit);
            }
            huff.incFreq(c);
        }

        // DECODE
        auto b = enc.cbegin();
        auto e = enc.cend();

        std::vector<Huffman::symbol_t> dec;
        while (b != e) {
            const auto symbol = huff2.decode(b, e);
            assert(b <= e);
            dec.push_back(symbol);
            huff2.incFreq(symbol);
        }
        REQUIRE(vec == dec);
    }
}

TEST_CASE("Handling 0-byte correctly", "[null-terminator]") {
    /* Okay... it actually turned out this was a "bug" in the decompress.cc
     * code, where if the sequence ends with a zero byte it doesn't decompress
     * correctly. (due to the early-break-out-of-loop-on-EOF-token thing.)
     * (So... it's not actually a bug at all.)
     * But this thing only surfaces anyway in the case where we have a vector
     * of bytes that happens to end with a \0, since iterating over a c-style
     * string won't give us the \0 anyway so it doesn't matter. Still, I guess
     * this was good to track down. */
    auto huff = Huffman();
    auto huff2 = Huffman();
    // Generate a random string of length i, encode it, then decode it
    std::vector<Huffman::symbol_t> vec;
    for (unsigned int j = 0; j < 100; ++j) {
        if (j == 40 || j == 99) {
            vec.push_back(0);
        } else {
            vec.push_back(j);
        }
    }
    // ENCODE
    Huffman::encoding_t enc;
    for (auto c : vec) {
        for (auto bit : huff.encode(c)) {
            enc.push_back(bit);
        }
        huff.incFreq(c);
    }

    // DECODE
    auto b = enc.cbegin();
    auto e = enc.cend();

    std::vector<Huffman::symbol_t> dec;
    while (b != e) {
        const auto symbol = huff2.decode(b, e);
        assert(b <= e);
        dec.push_back(symbol);
        huff2.incFreq(symbol);
    }
    REQUIRE(vec == dec);
}

