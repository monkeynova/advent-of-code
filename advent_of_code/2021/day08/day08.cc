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
  VLOG(1) << DebugSet(possible) << " - " << set;
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

absl::StatusOr<absl::flat_hash_map<char, char>> FindMap(const std::vector<absl::string_view>& left, const std::vector<absl::string_view>& right) {
  absl::flat_hash_map<char, char> ret;
  if (!TryMap(ret, 'a', left)) return AdventDay::Error("Could not find");
  return ret;


  absl::flat_hash_map<char, absl::flat_hash_set<char>> possible;

  for (char c1 : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
    for (char c2 : {'a', 'b', 'c', 'd', 'e', 'f', 'g'}) {
      possible[c1].insert(c2);
    }
  }
  for (absl::string_view test : left) {
    VLOG(1) << "test: " << test;
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
  for (absl::string_view test : right) {
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
  for (const auto& [c1, set] : possible) {
    VLOG(1) << c1 << ": " << DebugSet(set);
  }
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
    absl::StatusOr<absl::flat_hash_map<char, char>> map = FindMap(left, right);
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
