#include <unordered_map>
#include <queue>

#include "ptrtree.hh"
#include "huffman.hh"

namespace huffman {
    struct Huffman::Impl {
        std::unordered_map<symbol_t, int> charFreq;
        std::unique_ptr<tree::PtrTree> tree;
    };

    Huffman::Huffman() noexcept {
        pImpl_ = std::unique_ptr<Impl>(new Impl);
        for (int i = 0; i < 256; i++) {
            pImpl_->charFreq[static_cast<symbol_t>(i)] = 0;
        }
        recreate_tree();
    }

    void Huffman::incFreq(symbol_t symbol) {
        pImpl_->charFreq[symbol]++;

        recreate_tree();
    }

    Huffman::~Huffman() noexcept {
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
            if (pImpl_->tree->getByPath(path) < 256) {
                //printf("found a character: %d %c\n(path: %s)\n", pImpl_->tree->getByPath(path),pImpl_->tree->getByPath(path), path.c_str());
                return_value = pImpl_->tree->getByPath(path);
                begin = i + 1;
                break;
            }
        }
        return static_cast<symbol_t>(return_value);
    }

    Huffman::encoding_t Huffman::eofCode() const {
        encoding_t encode;
        encode.push_back(ZERO);
        return encode;
    }

    int Huffman::frequency(const tree::PtrTree* const tree) const {
        /* Get the weight of a given tree. If it represents a character,
         * look it up in the character frequency map; otherwise, just
         * return its key value. */
        if (tree->size() == 1) {
            return pImpl_->charFreq[static_cast<Huffman::symbol_t>(tree->getByPath(""))];
        } else {
            return tree->getByPath("") - 256;
        }
    }

    int Huffman::frequency(const tree::PtrTree& tree) const {
        /* Let it be called not using a pointer, for safety.
         * We need the pointer version sometimes because ptrtree
         * doesn't have a copy constructor so we can't call
         * frequency(*treeptr) */
        return frequency(&tree);
    }

    bool Huffman::compare_trees(const tree::PtrTree& left, const tree::PtrTree& right) const {
        return frequency(left) < frequency(right);
    }

    void Huffman::recreate_tree() {
        /* Our PtrTrees only support integers. To get around this, since
         * we're only encoding bytes, we can use numbers from 0-255 to
         * represent the encoded characters. We can tell which meaning is
         * meant because encoded characters always occur at leaf nodes --
         * so they will always have a size of 1. HOWEVER, we also want to
         * be able to use pathTo, which assumes the tree has unique keys.
         * So to avoid overlap, we'll add 256 to the nodes that represent
         * frequencies, and reserve 0-255 for only character values. */

        /* Unfortunately apparently they are a nightmare to put into
         * a priority queue, since the comparison depends on a member variable,
         * which is apparently anathema to C++. */

        auto compare = [&](const tree::PtrTree* const left, const tree::PtrTree* const right) {
            return frequency(left) > frequency(right);
        };

        std::priority_queue<tree::PtrTree*, std::vector<tree::PtrTree*>, decltype(compare)> forest(compare);

        /* First, we put all the individual nodes into the priority queue. */
        for (auto pair : pImpl_->charFreq) {
            forest.push(new tree::PtrTree(static_cast<int>(pair.first)));
        }

        /* Then, we repeat until we only have one tree... */
        while (forest.size() > 1) {
            /* get and remove top two elements */
            const tree::PtrTree *tree1 = forest.top();
            forest.pop();
            const tree::PtrTree *tree2 = forest.top();
            forest.pop();

            /* combine them into a new tree, and put it back into the forest */
            tree::PtrTree *newtree = new tree::PtrTree(frequency(tree1) + frequency(tree2) + 256, tree2, tree1);
            forest.push(newtree);
        }

        pImpl_->tree = std::unique_ptr<tree::PtrTree>(forest.top());
        //pImpl_->tree->print(0);
    }
}
