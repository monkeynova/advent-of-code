#include "advent_of_code/2015/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

enum Classification {
  kNaughty = 1,
  kNice = 2,
};

Classification Classify(absl::string_view input) {
  int vowel_count = 0;
  const absl::flat_hash_set<char> vowels = {'a', 'e', 'i', 'o', 'u'};
  bool has_double = false;
  const absl::flat_hash_set<absl::string_view> bad_pairs =
    {"ab", "cd", "pq", "xy"};
  bool has_bad_pair = false;

  for (int i = 0; i < input.size(); ++i) {
    if (vowels.contains(input[i])) ++vowel_count;
    if (i > 0) {
      if (input[i-1] == input[i]) has_double = true;
      if (bad_pairs.contains(input.substr(i - 1, 2))) has_bad_pair = true;
    }
  }

  if (vowel_count < 3) return kNaughty;
  if (!has_double) return kNaughty;
  if (has_bad_pair) return kNaughty;

  return kNice;
}

absl::StatusOr<std::vector<std::string>> Day05_2015::Part1(
    const std::vector<absl::string_view>& input) const {
  int nice = 0;
  for (absl::string_view str : input) {
    if (Classify(str) == kNice) ++nice;
  }
  return IntReturn(nice);
}

absl::StatusOr<std::vector<std::string>> Day05_2015::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}