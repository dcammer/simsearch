#pragma once
#include <numeric>
#include <vector>
#include <map>
#include <cassert>
#include "simsearch/minhash.hpp"

namespace simsearch {
template<typename K, typename V, size_t KW>
class LSH {
public:
  using MinHash=simsearch::MinHash<V, KW>;
private:
  using Band=std::map<size_t, std::vector<K>>;
  using Bands=std::vector<Band>;
  double min_similarity_;
  size_t row_size_;
  size_t band_count_;
  Bands bands_;
  std::vector<size_t> band_hashes(const MinHash& min_hash) const {
    std::vector<size_t> hashes(band_count_);
    for(size_t r=0; r < KW; r += row_size_) {
      auto begin = min_hash.hashes_.begin();
      // Input is already a uniform random distribution of bits
      auto band_hash = std::accumulate(std::next(begin, r), std::next(begin, r + row_size_),
          size_t(0), std::bit_xor<size_t>());
      hashes[r/row_size_]=band_hash;
    }
    return hashes;
  }
public:
  LSH(double similarity) : min_similarity_(similarity), row_size_(2),
    band_count_(KW/row_size_), bands_(band_count_)// TODO: Calculate the correct row_size_
  {
    // Similarity has to be between 0 and 1
    assert(similarity <= 1.0 && similarity >= 0.0);
  }

  bool insert(const K& name, const MinHash& min_hash) {
    auto bands = band_hashes(min_hash);
    int i = 0;
    for(size_t band_num=0; band_num < bands.size(); band_num++) {
      bands_[band_num][bands[band_num]].push_back(name);
      i++;
    }
    return false;
  }

  std::vector<K> matches(const MinHash& min_hash) const {
    std::set<K> elements;
    auto bands = band_hashes(min_hash);
    for(size_t band_num=0; band_num < bands.size(); band_num++) {
      const Band& band = bands_.at(band_num);
      size_t hash_value = bands[band_num];
      if (band.count(hash_value) > 0) {
        const auto& found = band.at(hash_value);
        std::copy(found.begin(), found.end(), std::inserter(elements, elements.begin()));
      }
    }
    return std::vector<K>(elements.begin(), elements.end());
  }
};
} // namespace simsearch
