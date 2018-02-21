#include <unordered_map>
#include <queue>

#include "ptrtree.hh"
#include "huffman.hh"

namespace huffman {
    constexpr int NUM_VALUES = 257;

    struct Huffman::Impl {
        std::unordered_map<int, int> charFreq;
        std::unordered_map<const tree::PtrTree*, int> depths;
        tree::PtrTree *tree;
    };

    Huffman::Huffman() noexcept {
        pImpl_ = std::unique_ptr<Impl>(new Impl);
        for (int i = 0; i < NUM_VALUES; i++) {
            pImpl_->charFreq[i] = 0;
        }
        pImpl_->tree = NULL;
        recreate_tree();
    }

    void Huffman::incFreq(symbol_t symbol) {
        pImpl_->charFreq[symbol]++;

        recreate_tree();
    }

    Huffman::~Huffman() noexcept {
        delete pImpl_->tree;
    }

    Huffman::encoding_t Huffman::encode(symbol_t c) const {
        encoding_t encoding;
        std::string path = pImpl_->tree->pathTo(static_cast<int>(c));
        for (auto ch : path) {
            if (ch == 'L') {
                encoding.push_back(ZERO);
            } else {
                encoding.push_back(ONE);
            }
        }
        return encoding;
    }

    Huffman::symbol_t Huffman::decode(enc_iter_t& begin, const enc_iter_t& end) const noexcept(false) {
        std::string path = "";
        int return_value = 0;
        for (auto i = begin; i != end; i++) {
            if (*i == ZERO) {
                path += "L";
            } else {
                path += "R";
            }
            auto value = pImpl_->tree->getByPath(path);
            if (value < NUM_VALUES) {
                if (value == NUM_VALUES-1) {
                    begin = end;
                } else {
                    return_value = value;
                    begin = i + 1;
                }
                break;
            }
        }
        return static_cast<symbol_t>(return_value);
    }

    Huffman::encoding_t Huffman::eofCode() const {
        encoding_t encoding;
        std::string path = pImpl_->tree->pathTo(NUM_VALUES-1);
        for (auto ch : path) {
            if (ch == 'L') {
                encoding.push_back(ZERO);
            } else {
                encoding.push_back(ONE);
            }
        }
        return encoding;
    }

    int Huffman::frequency(const tree::PtrTree* const tree) const {
        /* Get the weight of a given tree. If it represents a character,
         * look it up in the character frequency map; otherwise, just
         * return its key value. */
        if (tree->size() == 1) {
            return pImpl_->charFreq[static_cast<Huffman::symbol_t>(tree->getByPath(""))];
        } else {
            return tree->getByPath("") - NUM_VALUES;
        }
    }

    int Huffman::frequency(const tree::PtrTree& tree) const {
        /* Let it be called not using a pointer, for safety.
         * We need the pointer version sometimes because ptrtree
         * doesn't have a copy constructor so we can't call
         * frequency(*treeptr) */
        return frequency(&tree);
    }

    void Huffman::recreate_tree() {
        /* Our PtrTrees only support integers. To get around this, since
         * we're only encoding bytes, we can use numbers from 0-255 to
         * represent the encoded characters. We can tell which meaning is
         * meant because encoded characters always occur at leaf nodes --
         * so they will always have a size of 1. HOWEVER, we also want to
         * be able to use pathTo, which assumes the tree has unique keys.
         * So to avoid overlap, we'll add NUM_VALUES (= 257) to the nodes
         * that represent frequencies, and reserve 0-255 for only character
         * values. (256 represents the EOF character.) */

        /* (we don't need to worry about the duplicate keys when talking about
         * frequencies, because we never look up tree nodes by frequency -- it
         * only matters that all possible character values are distinct from
         * any frequency value.) */

        /* Unfortunately apparently they are a nightmare to put into
         * a priority queue, since the comparison depends on a member variable,
         * which is apparently anathema to C++. */

        auto compare = [&](const tree::PtrTree* const left, const tree::PtrTree* const right) {
            if (frequency(left) == frequency(right)) {
                /* Use tree depth as a "tiebreaker", to cause trees to be
                 * more well-balanced when they have a bunch of zeroes. This
                 * will reduce the length of codes for symbols we're seeing
                 * for the first time. */
                return pImpl_->depths[left] > pImpl_->depths[right];
            } else {
                return frequency(left) > frequency(right);
            }
        };

        std::priority_queue<tree::PtrTree*, std::vector<tree::PtrTree*>, decltype(compare)> forest(compare);

        /* First, we put all the individual nodes into the priority queue. */
        for (auto pair : pImpl_->charFreq) {
            tree::PtrTree *basetree = new tree::PtrTree(static_cast<int>(pair.first));
            pImpl_->depths[basetree] = 0;
            forest.push(basetree);
        }

        /* Then, we repeat until we only have one tree... */
        while (forest.size() > 1) {
            /* get and remove top two elements */
            const tree::PtrTree *tree1 = forest.top();
            forest.pop();
            const tree::PtrTree *tree2 = forest.top();
            forest.pop();

            /* combine them into a new tree, and put it back into the forest */
            tree::PtrTree *newtree = new tree::PtrTree(frequency(tree1) + frequency(tree2) + NUM_VALUES, tree2, tree1);
            pImpl_->depths[newtree] = std::max(pImpl_->depths[tree1], pImpl_->depths[tree2]) + 1;
            forest.push(newtree);
        }

        delete pImpl_->tree;
        pImpl_->tree = forest.top();
    }
}
