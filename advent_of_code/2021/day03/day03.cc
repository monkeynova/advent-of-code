#include "advent_of_code/2021/day03/day03.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_03::Part1(
    absl::Span<absl::string_view> input) const {
  int64_t gamma = 0;
  int64_t epsilon = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    int64_t ones = 0;
    int64_t zeros = 0;
    for (absl::string_view rec : input) {
      if (rec.size() <= i) return Error("Bad line: ", rec);
      if (rec[i] == '1')
        ++ones;
      else if (rec[i] == '0')
        ++zeros;
      else
        return Error("Bad bit");
    }
    if (ones == zeros) return Error("Same rate");
    if (ones > zeros) {
      gamma = gamma * 2 + 1;
      epsilon *= 2;
    } else {
      gamma *= 2;
      epsilon = epsilon * 2 + 1;
    }
  }
  return IntReturn(gamma * epsilon);
}

absl::StatusOr<std::string> Day_2021_03::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<absl::string_view> o2_set(input.begin(), input.end());
  for (int i = 0; i < input[0].size(); ++i) {
    int64_t ones = 0;
    int64_t zeros = 0;
    for (absl::string_view rec : o2_set) {
      if (rec.size() <= i) return Error("Bad line: ", rec);
      if (rec[i] == '1')
        ++ones;
      else if (rec[i] == '0')
        ++zeros;
      else
        return Error("Bad bit");
    }
    char delete_char = ones >= zeros ? '0' : '1';
    for (auto it = o2_set.begin(); it != o2_set.end();) {
      if ((*it)[i] == delete_char)
        o2_set.erase(it++);
      else
        ++it;
    }

    if (o2_set.size() == 1) break;
  }
  if (o2_set.size() != 1) return Error("Cannot find o2_set");

  absl::flat_hash_set<absl::string_view> co2_set(input.begin(), input.end());
  for (int i = 0; i < input[0].size(); ++i) {
    int64_t ones = 0;
    int64_t zeros = 0;
    for (absl::string_view rec : co2_set) {
      if (rec.size() <= i) return Error("Bad line: ", rec);
      if (rec[i] == '1')
        ++ones;
      else if (rec[i] == '0')
        ++zeros;
      else
        return Error("Bad bit");
    }
    char delete_char = ones >= zeros ? '1' : '0';
    for (auto it = co2_set.begin(); it != co2_set.end();) {
      if ((*it)[i] == delete_char)
        co2_set.erase(it++);
      else
        ++it;
    }

    if (co2_set.size() == 1) break;
  }
  if (co2_set.size() != 1) return Error("Cannot find co2_set");

  int64_t o2_val = 0;
  for (char c : *o2_set.begin()) {
    o2_val = o2_val * 2 + (c == '1' ? 1 : 0);
  }
  int64_t co2_val = 0;
  for (char c : *co2_set.begin()) {
    co2_val = co2_val * 2 + (c == '1' ? 1 : 0);
  }
  return IntReturn(o2_val * co2_val);
}

}  // namespace advent_of_code
