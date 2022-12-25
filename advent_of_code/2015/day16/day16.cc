#include "advent_of_code/2015/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_16::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> features = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};
  absl::flat_hash_set<int> aunts;
  for (int i = 1; i <= 500; ++i) aunts.insert(i);
  for (absl::string_view str : input) {
    int id, v1, v2, v3;
    absl::string_view f1, f2, f3;
    // Sue 1: children: 1, cars: 8, vizslas: 7
    if (!RE2::FullMatch(str,
                        "Sue (\\d+): (.*): (\\d+), (.*): (\\d+), (.*): (\\d+)",
                        &id, &f1, &v1, &f2, &v2, &f3, &v3)) {
      return Error("Bad input: ", str);
    }
    for (auto pair : std::vector<std::pair<absl::string_view, int>>{
             {f1, v1}, {f2, v2}, {f3, v3}}) {
      if (features[pair.first] != pair.second) aunts.erase(id);
    }
  }
  if (aunts.size() != 1) return Error("Aunt not unique: ", aunts.size());
  return AdventReturn(*aunts.begin());
}

absl::StatusOr<std::string> Day_2015_16::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> features = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};
  absl::flat_hash_set<int> aunts;
  for (int i = 1; i <= 500; ++i) aunts.insert(i);
  for (absl::string_view str : input) {
    int id, v1, v2, v3;
    absl::string_view f1, f2, f3;
    // Sue 1: children: 1, cars: 8, vizslas: 7
    if (!RE2::FullMatch(str,
                        "Sue (\\d+): (.*): (\\d+), (.*): (\\d+), (.*): (\\d+)",
                        &id, &f1, &v1, &f2, &v2, &f3, &v3)) {
      return Error("Bad input: ", str);
    }
    for (auto pair : std::vector<std::pair<absl::string_view, int>>{
             {f1, v1}, {f2, v2}, {f3, v3}}) {
      if (pair.first == "cats" || pair.first == "trees") {
        if (features[pair.first] >= pair.second) aunts.erase(id);
      } else if (pair.first == "pomeranians" || pair.first == "goldfish") {
        if (features[pair.first] <= pair.second) aunts.erase(id);
      } else {
        if (features[pair.first] != pair.second) aunts.erase(id);
      }
    }
  }
  if (aunts.size() != 1) return Error("Aunt not unique: ", aunts.size());
  return AdventReturn(*aunts.begin());
}

}  // namespace advent_of_code
