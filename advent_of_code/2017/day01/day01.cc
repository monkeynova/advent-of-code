#include "advent_of_code/2017/day01/day01.h"

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

absl::StatusOr<std::vector<std::string>> Day01_2017::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int sum = 0;
  int size = input[0].size();
  for (int i = 0; i < size; ++i) {
    if (input[0][i] == input[0][(i+1) % size]) {
      sum += input[0][i] - '0';
    }
  }
  return IntReturn(sum);
}

absl::StatusOr<std::vector<std::string>> Day01_2017::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int sum = 0;
  int size = input[0].size();
  for (int i = 0; i < size; ++i) {
    if (input[0][i] == input[0][(i + (size/2)) % size]) {
      sum += input[0][i] - '0';
    }
  }
  return IntReturn(sum);
}

}  // namespace advent_of_code
