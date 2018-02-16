/*
 * PtrTree: a tree implementation using pointers
 * by Miles Baker
 */

#include "ptrtree.hh"

namespace tree {

    PtrTree::PtrTree(value_t value) {
        value_ = value;
        left_ = nullptr;
        right_ = nullptr;
        size_ = 1;
    }

    PtrTree::PtrTree(value_t newroot, const PtrTree& left, const PtrTree& right) {
        value_ = newroot;
        left_ = &left;
        right_ = &right;
        size_ = 1;
        size_ += left_->size_;
        size_ += right_->size_;
    }

    PtrTree::PtrTree(value_t newroot, const PtrTree* const left,
            const PtrTree* const right) {
        value_ = newroot;
        left_ = left;
        right_ = right;
        size_ = 1;
        if (left_ != nullptr) {
            size_ += left_->size_;
        }
        if (right_ != nullptr) {
            size_ += right_->size_;
        }
    }

    unsigned PtrTree::size() const {
        return size_;
    }

    std::string PtrTree::pathTo(value_t value) const {
        std::string result = getPathTo(value);
        if (result == "no") {
            throw std::runtime_error("value not found in tree!");
        } else {
            return result;
        }
    }

    std::string PtrTree::getPathTo(value_t value) const {
        if (value_ == value) {
            return "";
        } else {
            // look in the children for the value, if they exist
            std::string left_path = "no", right_path = "no";

            if (left_ != nullptr) {
                left_path = left_->getPathTo(value);
                if (left_path != "no") {
                    left_path.insert(left_path.begin(), 'L');
                }
            }

            if (right_ != nullptr) {
                right_path = right_->getPathTo(value);
                if (right_path != "no") {
                    right_path.insert(right_path.begin(), 'R');
                }
            }

            if (left_path != "no") {
                // we have a bias to the left, here, if there are two
                // duplicate values in the tree
                return left_path;
            } else if (right_path != "no") {
                return right_path;
            } else {
                return std::string("no");
            }
        }
    }

    PtrTree::value_t PtrTree::getByPath(const std::string& path) const {
        if (path.length() == 0) {
            return value_;
        }
        char direction = path[0];
        if (direction == 'L') {
            if (left_ != nullptr) {
                return left_->getByPath(path.substr(1));
            } else {
                throw std::runtime_error("Value not in tree!");
            }
        } else if (direction == 'R') {
            if (right_ != nullptr) {
                return right_->getByPath(path.substr(1));
            } else {
                throw std::runtime_error("Value not in tree!");
            }
        } else {
            throw std::runtime_error("invalid character in path!");
        }
    }

} // namespace tree
