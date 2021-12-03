#include "advent_of_code/2021/day03/day03.h"

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
      if (rec[i] == '1') ++ones;
      else if (rec[i] == '0') ++zeros;
      else return Error("Bad bit");
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
  absl::flat_hash_set<absl::string_view> most(input.begin(), input.end());
  for (int i = 0; i < input[0].size(); ++i) {
    int64_t ones = 0;
    int64_t zeros = 0;
    for (absl::string_view rec : most) {
      if (rec.size() <= i) return Error("Bad line: ", rec);
      if (rec[i] == '1') ++ones;
      else if (rec[i] == '0') ++zeros;
      else return Error("Bad bit");
    }
    char delete_char = ones >= zeros ? '0' : '1';
    for (auto it = most.begin(); it != most.end();) {
      if ((*it)[i] == delete_char) most.erase(it++);
      else ++it;
    }

    if (most.size() == 1) break;    
  }
  if (most.size() != 1) return Error("Cannot find most");

  absl::flat_hash_set<absl::string_view> least(input.begin(), input.end());
  for (int i = 0; i < input[0].size(); ++i) {
    int64_t ones = 0;
    int64_t zeros = 0;
    for (absl::string_view rec : least) {
      if (rec.size() <= i) return Error("Bad line: ", rec);
      if (rec[i] == '1') ++ones;
      else if (rec[i] == '0') ++zeros;
      else return Error("Bad bit");
    }
    char delete_char = ones >= zeros ? '1' : '0';
    for (auto it = least.begin(); it != least.end();) {
      if ((*it)[i] == delete_char) least.erase(it++);
      else ++it;
    }

    if (least.size() == 1) break;    
  }
  if (least.size() != 1) return Error("Cannot find least");

  int64_t most_val = 0;
  for (char c : *most.begin()) {
    most_val = most_val * 2 + (c == '1' ? 1 : 0);
  }
  int64_t least_val = 0;
  for (char c : *least.begin()) {
    least_val = least_val * 2 + (c == '1' ? 1 : 0);
  }
  return IntReturn(most_val * least_val);
}

}  // namespace advent_of_code
