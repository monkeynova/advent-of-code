#include "advent_of_code/2019/day07/day07.h"

#include <algorithm>

#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"

namespace advent_of_code {
namespace {

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

    ASSIGN_OR_RETURN(
        int sub_best_thrust,
        FindBestThrust(base_codes, output[0], index + 1, used));
    best_thrust = std::max(best_thrust, sub_best_thrust);
    used.erase(i);
  }
  return best_thrust;
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

  std::optional<int64_t> output() { return output_; }
  std::optional<int64_t> last_nonempty_output() {
    return last_nonempty_output_;
  }

 private:
  std::vector<int64_t> input_;
  int input_pos_ = 0;
  std::optional<int64_t> output_;
  std::optional<int64_t> last_nonempty_output_;
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
    assembly.push_back(
        {.code = base_codes.Clone(), .io = AssemblyIO({phases[i]})});
  }

  assembly[0].io.add_input(0);
  bool running = true;
  while (running) {
    for (int i = 0; i < 5; ++i) {
      PartialState& unit = assembly[i];
      unit.io.clear_output();
      RETURN_IF_ERROR(unit.code.Run(&unit.io));
      if (unit.io.output()) {
        assembly[(i + 1) % 5].io.add_input(*unit.io.output());
      } else {
        running = false;
      }
    }
  }

  // Wait for termination on assembly.
  for (auto& unit : assembly) {
    RETURN_IF_ERROR(unit.code.Run());
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
    ASSIGN_OR_RETURN(
        int sub_best_thrust,
        FindBestThrustFeedback(base_codes, phases));
    best_thrust = std::max(best_thrust, sub_best_thrust);
    phases.pop_back();
  }
  return best_thrust;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_07::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  return AdventReturn(FindBestThrust(codes, 0));
}

absl::StatusOr<std::string> Day_2019_07::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  return AdventReturn(FindBestThrustFeedback(codes));
}

}  // namespace advent_of_code
