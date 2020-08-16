#include "simsearch/minhash.hpp"
#include "simsearch/lsh.hpp"
#include <chrono>
#include <random>
#include <fmt/chrono.h>
#include <iostream>

template<typename Rep, typename Period=std::ratio<1>>
static std::ostream& operator<<(std::ostream& out,
    const std::chrono::duration<Rep, Period> &value) {
  return  out << fmt::format("{}", value);
}
#include <catch2/catch.hpp> // this brings a lot of dependencies. we want our headers before that
using namespace std;
using namespace simsearch;

static const size_t SAMPLE_WIDTH = 32;
static mt19937 g_generator;
MinHash<unsigned int, SAMPLE_WIDTH> generate_random(size_t size) {
  MinHash<unsigned int, SAMPLE_WIDTH> min_hash;
  for(size_t j=0; j < size; j++) {
    min_hash.insert(g_generator());
  }
  return min_hash;
}


TEST_CASE("my_minhash") {
  SECTION("jaccard") {
    set<int> lhs({0, 1});
    set<int> rhs({1, 2});
    REQUIRE(jaccard_index(lhs, rhs) == Approx(0.333).margin(0.04));
  }
  SECTION("MinHash") {
    MinHash<int, 256> lhs({0, 1});
    MinHash<int, 256> rhs({1, 2});
    REQUIRE(lhs.jaccard(rhs) == Approx(0.33).margin(0.002));
  }
  SECTION("stringValues") {
    MinHash<string, 128> lhs({
        "dcammer@gmail.com", "scammer@gmail.com",
        "may@day.com"});
    MinHash<string, 128> rhs({"mary@night.com", 
        "scammer@gmail.com", "piano@pei.com"});
    REQUIRE(lhs.jaccard(rhs) == Approx(0.2).margin(0.01));
  }
  SECTION("differentLists") {
    MinHash<string, 128> first({
            "dcammer@gmail.com",
            "may@day.com"
        });
    MinHash<string, 128> different(
        {"ramona@gmail.com", "quiby@gmail.com"});
    REQUIRE(first.jaccard(different) == Approx(0.0));
  }
}

TEST_CASE("lsh") {
  SECTION("LSH Lookup") {
    using Container=LSH<string, string, 128>;
    Container container(0.05);
    Container::MinHash saved(
        { "dcammer@gmail.com", "may@day.com" });
    container.insert("list_one", saved);
    Container::MinHash different(
        {"ramona@gmail.com", "quiby@gmail.com"});
    REQUIRE(different.jaccard(saved) == Approx(0.0));
    REQUIRE(container.matches(different) == vector<string>());
    Container::MinHash similar(
        {"dcammer@gmail.com", "quiby@gmail.com"});
    REQUIRE(container.matches(similar) == vector<string>({"list_one"}));
  }

  SECTION("Large list") {
    using Container=LSH<string, unsigned int, SAMPLE_WIDTH>;
    mt19937 generator;
    Container container(0.1);
    for(size_t i=0; i < 40000; i++) {
      container.insert(fmt::format("{}", i), generate_random(1));
    }
    auto start = chrono::steady_clock::now();
    REQUIRE(container.matches(generate_random(40)) == vector<string>());
    auto end = chrono::steady_clock::now();
    auto duration = end - start;
    REQUIRE(duration < 1ms);
  }
}
