#include "advent_of_code/2015/day05/day05.h"

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

enum Classification {
  kNaughty = 1,
  kNice = 2,
};

Classification Classify1(std::string_view input) {
  int vowel_count = 0;
  const absl::flat_hash_set<char> vowels = {'a', 'e', 'i', 'o', 'u'};
  bool has_double = false;
  const absl::flat_hash_set<std::string_view> bad_pairs = {"ab", "cd", "pq",
                                                           "xy"};
  bool has_bad_pair = false;

  for (int i = 0; i < input.size(); ++i) {
    if (vowels.contains(input[i])) ++vowel_count;
    if (i > 0) {
      if (input[i - 1] == input[i]) has_double = true;
      if (bad_pairs.contains(input.substr(i - 1, 2))) has_bad_pair = true;
    }
  }

  if (vowel_count < 3) return kNaughty;
  if (!has_double) return kNaughty;
  if (has_bad_pair) return kNaughty;

  return kNice;
}

Classification Classify2(std::string_view input) {
  bool found_repeat_around = false;
  for (int i = 2; i < input.size(); ++i) {
    if (input[i] == input[i - 2]) {
      found_repeat_around = true;
      break;
    }
  }
  if (!found_repeat_around) return kNaughty;

  bool found_dupe = false;
  for (int i = 0; i < input.size(); ++i) {
    for (int j = i + 2; j < input.size(); ++j) {
      if (input.substr(i, 2) == input.substr(j, 2)) {
        found_dupe = true;
        break;
      }
    }
  }
  if (!found_dupe) return kNaughty;

  return kNice;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_05::Part1(
    absl::Span<std::string_view> input) const {
  int nice = 0;
  for (std::string_view str : input) {
    if (Classify1(str) == kNice) ++nice;
  }
  return AdventReturn(nice);
}

absl::StatusOr<std::string> Day_2015_05::Part2(
    absl::Span<std::string_view> input) const {
  int nice = 0;
  for (std::string_view str : input) {
    if (Classify2(str) == kNice) ++nice;
  }
  return AdventReturn(nice);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_05()); });

}  // namespace advent_of_code
