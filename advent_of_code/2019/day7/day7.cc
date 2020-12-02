#include "advent_of_code/2019/day7/day7.h"

#include <algorithm>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

absl::StatusOr<int> FindBestThrust(const IntCode& base_codes, int input_value,
                                   int index = 0,
                                   // Copy since we're going to mutate.
                                   absl::flat_hash_set<int> used = {}) {
  if (used.size() == 5) return input_value;

  int best_thrust = 0;
  for (int i = 0; i < 5; ++i) {
    if (used.contains(i)) continue;
    used.insert(i);
    IntCode clean_codes = base_codes.Clone();
    std::vector<int64_t> input = {i, input_value};
    std::vector<int64_t> output;
    absl::Status st = clean_codes.Run(input, &output);
    if (!st.ok()) return st;
    if (output.size() != 1) {
      return absl::InvalidArgumentError("Didn't return a single output");
    }

    absl::StatusOr<int> sub_best_thrust =
        FindBestThrust(base_codes, output[0], index + 1, used);
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

class CollectOutputAndPause : public IntCode::OutputSink, public IntCode::PauseCondition {
 public:
  bool PauseIntCode() override { return output_.has_value(); }

  absl::Status Put(int64_t val) override {
    output_ = val;
    return absl::OkStatus();
  }

  absl::optional<int64_t> output() {
    return output_;
  }

 private:
  absl::optional<int64_t> output_;
};

absl::StatusOr<int> RunAssembly(const IntCode& base_codes,
                                std::vector<int> phases) {
  struct PartialState {
    IntCode code;
    std::vector<int64_t> input;
  };

  // Initialize.
  std::vector<PartialState> assembly;
  for (int i = 0; i < 5; ++i) {
    assembly.push_back({.code = base_codes.Clone(), .input = {phases[i]}});
  }

  absl::optional<int64_t> next_input = 0;
  bool running = true;
  int64_t last_output = -1;
  while (running) {
    for (int i = 0; i < 5; ++i) {
      PartialState& unit = assembly[i];
      if (next_input) {
        unit.input.push_back(*next_input);
      }
      IntCode::VectorInput input(unit.input);
      CollectOutputAndPause out_and_pause;
      if (absl::Status st = unit.code.Run(&input, &out_and_pause, &out_and_pause); !st.ok()) {
        return st;
      }
      unit.input.clear();
      if (out_and_pause.output()) {
        next_input = *out_and_pause.output();
      } else {
        next_input = absl::nullopt;
        running = false;
      }
    }
    if (next_input) {
      last_output = *next_input;
    }
  }

  // Wait for termination on assembly.
  for (auto& unit : assembly) {
    if (absl::Status st = unit.code.Run(); !st.ok()) return st;
  }

  return last_output;
}

absl::StatusOr<int> FindBestThrustFeedback(const IntCode& base_codes,
                                           // Copy since we're going to mutate.
                                           std::vector<int> phases = {}) {
  if (phases.size() == 5) {
    return RunAssembly(base_codes, phases);
  }

  int best_thrust = 0;
  for (int i = 5; i < 10; ++i) {
    if (std::any_of(phases.begin(), phases.end(),
                    [i](int p) { return i == p; }))
      continue;
    phases.push_back(i);
    absl::StatusOr<int> sub_best_thrust =
        FindBestThrustFeedback(base_codes, phases);
    if (!sub_best_thrust.ok()) return sub_best_thrust.status();
    best_thrust = std::max(best_thrust, *sub_best_thrust);
    phases.pop_back();
  }
  return best_thrust;
}

absl::StatusOr<std::vector<std::string>> Day7_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  absl::StatusOr<int> best_result = FindBestThrustFeedback(*codes);
  if (!best_result.ok()) return best_result.status();

  return std::vector<std::string>{absl::StrCat(*best_result)};
}
