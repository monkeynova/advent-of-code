#include "advent_of_code/2016/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "glog/logging.h"
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

absl::StatusOr<std::vector<std::string>> Day25_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  for (int a = 0; true; ++a) {
    AssemBunny copy = *vm;
    VLOG(1) << "Trying: " << a;
    copy.registers().a = a;
    OutputWatch watch;
    if (absl::Status st = copy.Execute(&watch, &watch); !st.ok()) return st;
    bool check = true;
    const std::vector<int64_t>& output = watch.output();
    VLOG(1) << " output: " << absl::StrJoin(output, ",");
    for (int i = 0; check && i < output.size(); ++i) {
      if (output[i] != 0 && output[i] != 1) check = false;
      if (output[i] == output[(i + 1) % output.size()]) check = false;
    }
    if (check) {
      return IntReturn(a);
    }
  }
  return Error("Impossible");
}

absl::StatusOr<std::vector<std::string>> Day25_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return std::vector<std::string>{"Merry Christmas"};
}

}  // namespace advent_of_code
