#include "advent_of_code/2021/day08/day08.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::string DebugSet(const absl::flat_hash_set<char>& possible) {
  std::string ret;
  for (char c : possible) ret.append(1, c);
  return ret;
}

absl::flat_hash_set<char> Restrict(const absl::flat_hash_set<char>& possible, absl::string_view set) {
  absl::flat_hash_set<char> new_possible;
  for (char c : set) {
    if (possible.contains(c)) new_possible.insert(c);
  }
  return new_possible;
}

static const absl::flat_hash_map<std::string, int64_t> kLookup = {
  {"abcefg", 0}, {"cf", 1}, {"acdeg", 2}, {"acdfg", 3}, {"bcdf", 4},
  {"abdfg", 5}, {"abdefg", 6}, {"acf", 7}, {"abcdefg", 8}, {"abcdfg", 9},
};

absl::StatusOr<int64_t> Decode(absl::string_view digit, const absl::flat_hash_map<char, char>& map) {
  std::string digit_copy(digit);
  for (char& c : digit_copy) {
    auto it = map.find(c);
    if (it == map.end()) return AdventDay::Error("Could not find digit: ", digit_copy);
    c = it->second;
  }
  absl::c_sort(digit_copy);
  auto it = kLookup.find(digit_copy);
  if (it == kLookup.end()) return AdventDay::Error("Could not lookup digit_copy: ", digit_copy);
  return it->second;
}

bool TryMap(
    absl::flat_hash_map<char, char>& map, char next, const std::vector<absl::string_view>& digits) {
  if (next == 'h') {
    for (absl::string_view d : digits) {
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

absl::StatusOr<absl::flat_hash_map<char, char>> FindMap(const std::vector<absl::string_view>& left) {
  if (left.size() != 10) return AdventDay::Error("Bad exemplars");

  absl::flat_hash_map<int64_t, absl::flat_hash_set<char>> countToSegments = {
    {8, {'a', 'c'}}, {6, {'b'}}, {7, {'d', 'g'}}, {4, {'e'}}, {9, {'f'}},
  };

  absl::flat_hash_map<char, int64_t> observed;
  for (absl::string_view in : left) {
    for (char c : in) {
      ++observed[c];
    }
  }

  absl::flat_hash_map<char, absl::flat_hash_set<char>> possible;

  for (char c1 : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    auto it = observed.find(c1);
    if (it == observed.end()) return AdventDay::Error("Bad observed");
    auto it2 = countToSegments.find(it->second);
    if (it2 == countToSegments.end()) return AdventDay::Error("Bad count");
    possible[c1] = it2->second;
  }
  for (absl::string_view test : left) {
    switch (test.size()) {
      case 2: {
        possible[test[0]] = Restrict(possible[test[0]], "cf");
        possible[test[1]] = Restrict(possible[test[1]], "cf");
        break;
      }
      case 3: {
        possible[test[0]] = Restrict(possible[test[0]], "acf");
        possible[test[1]] = Restrict(possible[test[1]], "acf");
        possible[test[2]] = Restrict(possible[test[2]], "acf");
        break;
      }
      case 4: {
        possible[test[0]] = Restrict(possible[test[0]], "bcdf");
        possible[test[1]] = Restrict(possible[test[1]], "bcdf");
        possible[test[2]] = Restrict(possible[test[2]], "bcdf");
        possible[test[3]] = Restrict(possible[test[3]], "bcdf");
        break;
      }
    }
  }
  absl::flat_hash_set<char> definitive;
  for (const auto& [c1, set] : possible) {
    if (set.size() == 1) definitive.insert(*set.begin());
  }
  bool did_work = true;
  while (did_work) {
    did_work = false;
    for (auto& [c1, set] : possible) {
      if (set.size() > 1) {
        did_work = true;
        absl::flat_hash_set<char> new_set;
        for (char c : set) {
          if (!definitive.contains(c)) new_set.insert(c);
        }
        set = std::move(new_set);
      }
    }
  }
  absl::flat_hash_map<char, char> ret;
  for (const auto& [c, set] : possible) {
    if (set.size() != 1) return AdventDay::Error("Bad possible");
    ret[c] = *set.begin();
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_08::Part1(
    absl::Span<absl::string_view> input) const {
  int64_t count = 0;
  for (absl::string_view line : input) {
    std::pair<absl::string_view, absl::string_view> split = absl::StrSplit(line, " | ");
    for (absl::string_view segment : absl::StrSplit(split.second, " ")) {
      switch (segment.size()) {
        case 2:
        case 3:
        case 4:
        case 7:
        ++count;
        break;
      }
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2021_08::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t sum = 0;
  for (absl::string_view line : input) {
    std::pair<absl::string_view, absl::string_view> split = absl::StrSplit(line, " | ");
    std::vector<absl::string_view> left = absl::StrSplit(split.first, " ");
    std::vector<absl::string_view> right = absl::StrSplit(split.second, " ");
    absl::StatusOr<absl::flat_hash_map<char, char>> map = FindMap(left);
    if (!map.ok()) return Error("Could not make map ", map.status().message(), " for ", line);
    int64_t right_val = 0;
    for (absl::string_view digit : right) {
      absl::StatusOr<int64_t> v = Decode(digit, *map);
      if (!v.ok()) return v.status();
      right_val = right_val * 10 + *v;
    }
    sum += right_val;
  }
  return IntReturn(sum);
}

}  // namespace advent_of_code
