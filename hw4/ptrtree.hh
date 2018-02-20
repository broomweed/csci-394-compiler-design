
#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "tree.hh"

namespace tree {

class PtrTree : public Tree {
  public:
    PtrTree(value_t value);
    ~PtrTree() = default;
    PtrTree(value_t newroot, const PtrTree& left, const PtrTree& right);
    PtrTree(value_t newroot, const PtrTree* const left, const PtrTree* const right);

    virtual unsigned size() const override;

    virtual std::string pathTo(value_t value) const override;

    value_t getByPath(const std::string& path) const override;

    void print(int depth) const;

  private:
    value_t value_;
    const PtrTree *left_;
    const PtrTree *right_;
    unsigned int size_;

    std::string getPathTo(value_t value) const;
};

} // namespace
