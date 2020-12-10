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

  return IntReturn(FindBestThrust(*codes, 0));
}

class AssemblyIO : public IntCode::IOModule {
 public:
  explicit AssemblyIO(std::vector<int64_t> input) : input_(std::move(input)) {}

  bool PauseIntCode() override { return output_.has_value(); }

  void clear_output() { output_ = absl::nullopt; }

  void add_input(int64_t val) { input_.push_back(val); }

  absl::Status Put(int64_t val) override {
    output_ = val;
    last_nonempty_output_ = val;
    return absl::OkStatus();
  }

  absl::StatusOr<int64_t> Fetch() override {
    if (input_pos_ < input_.size()) return input_[input_pos_++];
    return absl::InvalidArgumentError("Input is exhausted");
  }

  absl::optional<int64_t> output() { return output_; }
  absl::optional<int64_t> last_nonempty_output() { return last_nonempty_output_; }

 private:
  std::vector<int64_t> input_;
  int input_pos_ = 0;
  absl::optional<int64_t> output_;
  absl::optional<int64_t> last_nonempty_output_;
};

absl::StatusOr<int> RunAssembly(const IntCode& base_codes,
                                std::vector<int> phases) {
  struct PartialState {
    IntCode code;
    AssemblyIO io;
  };

  // Initialize.
  std::vector<PartialState> assembly;
  for (int i = 0; i < 5; ++i) {
    assembly.push_back({.code = base_codes.Clone(), .io = AssemblyIO({phases[i]})});
  }

  absl::optional<int64_t> next_input = 0;
  bool running = true;
  // TODO(@monkeynova): This whole mess would be much better implemented with
  // the IOModule interface. Parallel execution could be implemented like
  // day23.
  while (running) {
    for (int i = 0; i < 5; ++i) {
      PartialState& unit = assembly[i];
      if (next_input) {
        unit.io.add_input(*next_input);
      }
      unit.io.clear_output();
      if (absl::Status st = unit.code.Run(&unit.io); !st.ok()) {
        return st;
      }
      if (unit.io.output()) {
        next_input = *unit.io.output();
      } else {
        next_input = absl::nullopt;
        running = false;
      }
    }
  }

  // Wait for termination on assembly.
  for (auto& unit : assembly) {
    if (absl::Status st = unit.code.Run(); !st.ok()) return st;
  }

  if (!assembly.back().io.last_nonempty_output()) {
    return absl::InvalidArgumentError("No output");
  }

  return *assembly.back().io.last_nonempty_output();
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

  return IntReturn(FindBestThrustFeedback(*codes));
}
