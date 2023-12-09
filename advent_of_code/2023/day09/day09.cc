// http://adventofcode.com/2023/day/9

#include "advent_of_code/2023/day09/day09.h"

#include "absl/log/log.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2023_09::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(std::vector<int> vals, ParseAsInts<int>(line));
    int64_t predict = 0;
    for (bool done = false; !done; vals.pop_back()) {
      predict += vals.back();
      done = true;
      for (int i = 0; i + 1 < vals.size(); ++i) {
        vals[i] = vals[i + 1] - vals[i];
        done &= vals[i] == 0;
      }
    }
    total += predict;
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_09::Part2(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(std::vector<int> vals, ParseAsInts<int>(line));
    int64_t predict = 0;
    bool negate = true;
    for (bool done = false; !done; vals.pop_back()) {
      predict = vals[0] - predict;
      negate = !negate;
      done = true;
      for (int i = 0; i + 1 < vals.size(); ++i) {
        vals[i] = vals[i + 1] - vals[i];
        done &= vals[i] == 0;
      }
    }
    if (negate) {
      total -= predict;
    } else {
      total += predict;
    }
  }
  return AdventReturn(total);
}

}  // namespace advent_of_code
