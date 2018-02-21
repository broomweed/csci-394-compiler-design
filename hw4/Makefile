
CXX=g++-7   # Can switch to g++ if desired
# CXX=g++-6
CXXFLAGS=-g -Og -std=c++17 -Wall -pedantic -Wextra -Werror
#CXXFLAGS=-O3 -std=c++17 -Wall -pedantic -Wextra -Werror
LDFLAGS=$(CXXFLAGS)
LIBS=

all: test_huffman compress decompress bitcompress bitdecompress

compress: compress.o huffman.o ptrtree.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

decompress: decompress.o huffman.o ptrtree.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

bitcompress: bitcompress.o huffman.o ptrtree.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

bitdecompress: bitdecompress.o huffman.o ptrtree.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

test_huffman: test_huffman.o huffman.o ptrtree.o
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $^

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

test: all
	./test_huffman

clean:
	rm -f *.o compress decompress test_huffman
