#include "advent_of_code/2015/day25/day25.h"

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

int64_t ComputeCodeValue(int position) {
  int64_t val = 20151125;
  int64_t power_product = 252533;
  for (int bit = 0; (1 << bit) <= position; ++bit) {
    if (position & (1 << bit)) {
      val = (val * power_product) % 33554393;
    }
    power_product = (power_product * power_product) % 33554393;
  }
  VLOG(1) << "ComputeCodeValue(" << position << ") = " << val;
  return val;
}

int64_t ComputeCodePosition(int row, int col) {
  int64_t diag = row + col - 2;
  int64_t position = diag * (diag + 1) / 2 + (col - 1);
  VLOG(1) << "ComputeCodePosition(" << row << "," << col << ") = " << position;
  return position;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_25::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    struct TestVal {
      int row;
      int col;
      int expected;
    };
    std::vector<TestVal> tests = {{1, 1, 20151125},
                                  {2, 1, 31916031},
                                  {1, 2, 18749137},
                                  {3, 4, 7981243},
                                  {5, 6, 31663883}};
    for (TestVal t : tests) {
      int64_t computed = ComputeCodeValue(ComputeCodePosition(t.row, t.col));
      if (computed != t.expected) {
        return Error("Test failed: ", t.row, "x", t.col, " should be ",
                     t.expected, " but is ", computed);
      }
    }
  }

  if (input.size() != 1) return Error("Bad input size: ", input.size());
  int row;
  int col;
  if (!RE2::FullMatch(
          input[0],
          "To continue, please consult the code grid in the manual.  "
          "Enter the code at row (\\d+), column (\\d+).",
          &row, &col)) {
    return Error("Can't parse input");
  }
  return AdventReturn(ComputeCodeValue(ComputeCodePosition(row, col)));
}

absl::StatusOr<std::string> Day_2015_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
