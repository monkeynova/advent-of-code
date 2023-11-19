#include "advent_of_code/2021/day08/day08.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::string DebugSet(const absl::flat_hash_set<char>& possible) {
  std::string ret;
  for (char c : possible) ret.append(1, c);
  return ret;
}

absl::flat_hash_set<char> Restrict(const absl::flat_hash_set<char>& possible,
                                   std::string_view set) {
  absl::flat_hash_set<char> new_possible;
  for (char c : set) {
    if (possible.contains(c)) new_possible.insert(c);
  }
  return new_possible;
}

static const absl::flat_hash_map<std::string, int64_t> kLookup = {
    {"abcefg", 0}, {"cf", 1},     {"acdeg", 2}, {"acdfg", 3},   {"bcdf", 4},
    {"abdfg", 5},  {"abdefg", 6}, {"acf", 7},   {"abcdefg", 8}, {"abcdfg", 9},
};

absl::StatusOr<int64_t> Decode(std::string_view digit,
                               const absl::flat_hash_map<char, char>& map) {
  std::string digit_copy(digit);
  for (char& c : digit_copy) {
    auto it = map.find(c);
    if (it == map.end()) {
      return Error("Could not find digit: ", digit_copy);
    }
    c = it->second;
  }
  absl::c_sort(digit_copy);
  auto it = kLookup.find(digit_copy);
  if (it == kLookup.end()) {
    return Error("Could not lookup digit_copy: ", digit_copy);
  }
  return it->second;
}

bool TryMap(absl::flat_hash_map<char, char>& map, char next,
            const std::vector<std::string_view>& digits) {
  if (next == 'h') {
    for (std::string_view d : digits) {
      absl::StatusOr<int64_t> v = Decode(d, map);
      if (!v.ok()) return false;
    }
    return true;
  }

  for (char c : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    if (map.contains(c)) continue;
    map[c] = next;
    if (TryMap(map, next + 1, digits)) return true;
    map.erase(c);
  }
  return false;
}

absl::StatusOr<absl::flat_hash_map<char, char>> FindMap(
    const std::vector<std::string_view>& left) {
  if (left.size() != 10) return Error("Bad exemplars");

  absl::flat_hash_map<int64_t, absl::flat_hash_set<char>> countToSegments = {
      {8, {'a', 'c'}}, {6, {'b'}}, {7, {'d', 'g'}}, {4, {'e'}}, {9, {'f'}},
  };

  absl::flat_hash_map<char, int64_t> observed;
  for (std::string_view in : left) {
    for (char c : in) {
      ++observed[c];
    }
  }

  absl::flat_hash_map<char, absl::flat_hash_set<char>> possible;

  for (char c1 : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    auto it = observed.find(c1);
    if (it == observed.end()) return Error("Bad observed");
    auto it2 = countToSegments.find(it->second);
    if (it2 == countToSegments.end()) return Error("Bad count");
    possible[c1] = it2->second;
  }
  for (std::string_view test : left) {
    std::string_view allowed = "";
    switch (test.size()) {
      case 2:
        allowed = "cf";
        break;
      case 3:
        allowed = "acf";
        break;
      case 4:
        allowed = "bcdf";
        break;
    }
    if (allowed.empty()) continue;
    for (char c : test) {
      possible[c] = Restrict(possible[c], allowed);
    }
  }
  absl::flat_hash_set<char> definitive;
  for (const auto& [c1, set] : possible) {
    if (set.size() == 1) definitive.insert(*set.begin());
  }
  bool more_work = true;
  while (more_work) {
    more_work = false;
    for (auto& [c1, set] : possible) {
      if (set.size() > 1) {
        absl::flat_hash_set<char> new_set;
        for (char c : set) {
          if (!definitive.contains(c)) new_set.insert(c);
        }
        set = std::move(new_set);
        if (set.size() > 1) {
          more_work = true;
        }
      }
    }
  }
  absl::flat_hash_map<char, char> ret;
  for (const auto& [c, set] : possible) {
    if (set.size() != 1) return Error("Bad possible");
    ret[c] = *set.begin();
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_08::Part1(
    absl::Span<std::string_view> input) const {
  int64_t count = 0;
  absl::flat_hash_set<int64_t> find_sizes = {2, 3, 4, 7};
  for (std::string_view line : input) {
    const auto& [_, find] = PairSplit(line, " | ");
    for (std::string_view segment : absl::StrSplit(find, " ")) {
      if (find_sizes.contains(segment.size())) ++count;
    }
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2021_08::Part2(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    const auto& [exemplars, decode] = PairSplit(line, " | ");
    absl::StatusOr<absl::flat_hash_map<char, char>> map =
        FindMap(absl::StrSplit(exemplars, " "));
    if (!map.ok()) {
      return Error("Could not make map ", map.status().ToString(), " for ",
                   exemplars);
    }
    int64_t decode_val = 0;
    for (std::string_view digit : absl::StrSplit(decode, " ")) {
      ASSIGN_OR_RETURN(int64_t v, Decode(digit, *map));
      decode_val = decode_val * 10 + v;
    }
    sum += decode_val;
  }
  return AdventReturn(sum);
}

}  // namespace advent_of_code
