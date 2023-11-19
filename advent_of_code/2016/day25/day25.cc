#include "advent_of_code/2016/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class OutputWatch : public AssemBunny::OutputInterface,
                    public AssemBunny::PauseInterface {
 public:
  bool Pause() override { return done_; }

  absl::Status OnOutput(int64_t v, const AssemBunny* ab) override {
    const AssemBunny::Registers& registers = ab->registers();
    VLOG(2) << "Output: " << v << registers;
    if (output_history_.contains(registers)) {
      done_ = true;
      return absl::OkStatus();
    }
    output_.push_back(v);
    output_history_.insert(registers);
    return absl::OkStatus();
  }

  const std::vector<int64_t>& output() { return output_; }

 private:
  std::vector<int64_t> output_;
  absl::flat_hash_set<AssemBunny::Registers> output_history_;
  bool done_ = false;
};

}  // namespace

absl::StatusOr<std::string> Day_2016_25::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(AssemBunny vm, AssemBunny::Parse(input));
  for (int a = 0; true; ++a) {
    AssemBunny copy = vm.Clone();
    VLOG(1) << "Trying: " << a;
    copy.registers().a = a;
    OutputWatch watch;
    RETURN_IF_ERROR(copy.Execute(&watch, &watch));
    bool check = true;
    const std::vector<int64_t>& output = watch.output();
    VLOG(1) << " output: " << absl::StrJoin(output, ",");
    for (int i = 0; check && i < output.size(); ++i) {
      if (output[i] != 0 && output[i] != 1) check = false;
      if (output[i] == output[(i + 1) % output.size()]) check = false;
    }
    if (check) {
      return AdventReturn(a);
    }
  }
  return Error("Impossible");
}

absl::StatusOr<std::string> Day_2016_25::Part2(
    absl::Span<std::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
