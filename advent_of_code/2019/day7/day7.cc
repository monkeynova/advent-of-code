#include "advent_of_code/2019/day7/day7.h"

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

absl::StatusOr<int> FindBestThrust(const IntCode& base_codes,
                                  int input_value,
                                  int index = 0,
                                  // Copy since we're going to mutate.
                                  absl::flat_hash_set<int> used = {}) {
  if (used.size() == 5) return input_value;

  int best_thrust = 0;
  for (int i = 0; i < 5; ++i) {
    if (used.contains(i)) continue;
    used.insert(i);
    IntCode clean_codes = base_codes.Clone();
    std::vector<int> input = {i, input_value};
    std::vector<int> output;
    absl::Status st = clean_codes.Run(input, &output);
    if (!st.ok()) return st;
    if (output.size() != 1) {
      return absl::InvalidArgumentError("Didn't return a single output");
    }

    absl::StatusOr<int> sub_best_thrust = FindBestThrust(base_codes, output[0], index + 1, used);
    if (!sub_best_thrust.ok()) return sub_best_thrust.status();
    best_thrust = std::max(best_thrust, *sub_best_thrust);
    used.erase(i);
  }
  return best_thrust;
}

absl::StatusOr<std::vector<std::string>> Day7_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  absl::StatusOr<int> best_result = FindBestThrust(*codes, 0);
  if (!best_result.ok()) return best_result.status();

  return std::vector<std::string>{absl::StrCat(*best_result)};
}

absl::StatusOr<std::vector<std::string>> Day7_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
