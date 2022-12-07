// https://adventofcode.com/2016/day/25
//
// --- Day 25: Clock Signal ---
// ----------------------------
// 
// You open the door and find yourself on the roof. The city sprawls away
// from you for miles and miles.
// 
// There's not much time now - it's already Christmas, but you're nowhere
// near the North Pole, much too far to deliver these stars to the sleigh
// in time.
// 
// However, maybe the huge antenna up here can offer a solution. After
// all, the sleigh doesn't need the stars, exactly; it needs the timing
// data they provide, and you happen to have a massive signal generator
// right here.
// 
// You connect the stars you have to your prototype computer, connect
// that to the antenna, and begin the transmission.
// 
// Nothing happens.
// 
// You call the service number printed on the side of the antenna and
// quickly explain the situation. "I'm not sure what kind of equipment
// you have connected over there," he says, "but you need a clock
// signal." You try to explain that this is a signal for a clock.
// 
// "No, no, a clock signal - timing information so the antenna computer
// knows how to read the data you're sending it. An endless, alternating
// pattern of 0, 1, 0, 1, 0, 1, 0, 1, 0, 1...." He trails off.
// 
// You ask if the antenna can handle a clock signal at the frequency you
// would need to use for the data from the stars. "There's no way it can!
// The only antenna we've installed capable of that is on top of a
// top-secret Easter Bunny installation, and you're definitely not-" You
// hang up the phone.
// 
// You've extracted the antenna's clock signal generation assembunny code
// (your puzzle input); it looks mostly compatible with code you worked
// on just recently.
// 
// This antenna code, being a signal generator, uses one extra
// instruction:
// 
// * out x transmits x (either an integer or the value of a register)
// as the next value for the clock signal.
// 
// The code takes a value (via register a) that describes the signal to
// generate, but you're not sure how it's used. You'll have to find the
// input to produce the right signal through experimentation.
// 
// What is the lowest positive integer that can be used to initialize
// register a and cause the code to output a clock signal of 0, 1, 0, 1...
// repeating forever?
//
// --- Part Two ---
// ----------------
// 
// The antenna is ready. Now, all you need is the fifty stars required to
// generate the signal for the sleigh, but you don't have enough.
// 
// You look toward the sky in desperation... suddenly noticing that a
// lone star has been installed at the top of the antenna! Only 49 more
// to go.


#include "advent_of_code/2016/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "absl/log/log.h"
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  for (int a = 0; true; ++a) {
    AssemBunny copy = vm->Clone();
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

absl::StatusOr<std::string> Day_2016_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
