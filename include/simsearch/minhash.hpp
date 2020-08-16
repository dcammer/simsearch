#pragma once
#include <cassert>
#include <set>
#include <string>
#include <fmt/ranges.h>

namespace simsearch {
template<typename T>
std::set<T> intersect(const std::set<T> &lhs, const std::set<T> &rhs) {
  std::set<T> rv;
  std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), inserter(rv, rv.end()));
  return rv;
}


template<typename T>
std::set<T> set_union(const std::set<T> &lhs, const std::set<T> &rhs) {
  std::set<T> rv;
  std::set_union(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), inserter(rv, rv.end()));
  return rv;
}


template<typename C>
double jaccard_index(const C &lhs, const  C &rhs) {
  return intersect(lhs, rhs).size()/double(set_union(lhs, rhs).size());
}


template<typename HV, size_t W>
class MinHash {
  using Hasher=std::function<size_t(const HV&)>;
  using CType=std::array<typename Hasher::result_type, W>;
  std::array<Hasher, W> hashers_;
  
  Hasher build_hasher(size_t hashNum) {
    std::hash<std::string> hasher;
    std::string custom= fmt::format("f{}", hashNum) + "{}";
    return [custom, hasher](const HV &value) {
      auto customized = fmt::format(custom, value);
      return hasher(customized);
    };
  }

  void build_hashers() {
    for(size_t i=0u; i < W; i++) {
      hashers_[i] = build_hasher(i);
    }
  }
public:
  CType hashes_;
  MinHash() {
    build_hashers();
    hashes_.fill(std::numeric_limits<typename Hasher::result_type>::max());
  }

  MinHash(const std::set<HV>&& value_set) : MinHash() {
    insert(value_set);
  }

  void insert(const std::set<HV>& value_set) {
    for(const HV& v: value_set){
      insert(v);
    }
  }

  void insert(const HV& v) {
    assert(hashers_.size() == W);
    assert(hashes_.size() == W);
    for(size_t cur=0; cur < W; cur++) {
      const Hasher& hasher = hashers_[cur];
      auto hash_value = hasher(v);
      hashes_[cur] = std::min(hash_value, hashes_[cur]);
    }
  }

  double jaccard(const MinHash<HV, W> &other) const {
    size_t total = 0;
    for(size_t i=0u; i < W; i++) {
      if (hashes_[i] == other.hashes_[i]) {
        total++;
      }
    }
    return total/double(W);
  }
};

} // namespace simsearch
