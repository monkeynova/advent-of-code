// https://adventofcode.com/2019/day/25
//
// --- Day 25: Cryostasis ---
// --------------------------
//
// As you approach Santa's ship, your sensors report two important
// details:
//
// First, that you might be too late: the internal temperature is -40
// degrees.
//
// Second, that one faint life signature is somewhere on the ship.
//
// The airlock door is locked with a code; your best option is to send in
// a small droid to investigate the situation. You attach your ship to
// Santa's, break a small hole in the hull, and let the droid run in
// before you seal it up again. Before your ship starts freezing, you
// detach your ship and set it to automatically stay within range of
// Santa's ship.
//
// This droid can follow basic instructions and report on its
// surroundings; you can communicate with it through an Intcode program
// (your puzzle input) running on an ASCII-capable computer.
//
// As the droid moves through its environment, it will describe what it
// encounters. When it says Command?, you can give it a single
// instruction terminated with a newline (ASCII code 10). Possible
// instructions are:
//
// * Movement via north, south, east, or west.
//
// * To take an item the droid sees in the environment, use the command
// take <name of item>. For example, if the droid reports seeing a
// red ball, you can pick it up with take red ball.
//
// * To drop an item the droid is carrying, use the command drop <name
// of item>. For example, if the droid is carrying a green ball, you
// can drop it with drop green ball.
//
// * To get a list of all of the items the droid is currently carrying,
// use the command inv (for "inventory").
//
// Extra spaces or other characters aren't allowed - instructions must be
// provided precisely.
//
// Santa's ship is a Reindeer-class starship; these ships use
// pressure-sensitive floors to determine the identity of droids and crew
// members. The standard configuration for these starships is for all
// droids to weigh exactly the same amount to make them easier to detect.
// If you need to get past such a sensor, you might be able to reach the
// correct weight by carrying items from the environment.
//
// Look around the ship and see if you can find the password for the main
// airlock.
//
// --- Part Two ---
// ----------------
//
// As you move through the main airlock, the air inside the ship is
// already heating up to reasonable levels. Santa explains that he didn't
// notice you coming because he was just taking a quick nap. The ship
// wasn't frozen; he just had the thermostat set to "North Pole".
//
// You make your way over to the navigation console. It beeps. "Status:
// Stranded. Please supply measurements from 49 stars to recalibrate."
//
// "49 stars? But the Elves told me you needed fifty--"
//
// Santa just smiles and nods his head toward the window. There, in the
// distance, you can see the center of the Solar System: the Sun!
//
// The navigation console beeps again.

#include "advent_of_code/2019/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

class Terminal : public IntCode::IOModule {
 public:
  explicit Terminal(std::string input) : input_(input) {}

  bool PauseIntCode() override { return done_; }

  absl::StatusOr<int64_t> Fetch() override {
    if (input_pos_ < input_.size()) return input_[input_pos_++];
    return absl::InvalidArgumentError("Input exhausted");
    input_pos_ = 0;
    return input_[input_pos_++];
  }

  absl::Status Put(int64_t val) override {
    if (val == '\n') {
      LOG(INFO) << output_;
      absl::StrAppend(&full_output_, output_, "\n");
      output_.clear();
    } else {
      char val_str[] = {static_cast<char>(val), '\0'};
      absl::StrAppend(&output_, val_str);
    }
    return absl::OkStatus();
  }

  const std::string& full_output() { return full_output_; }

 private:
  bool done_ = false;
  std::string input_;
  int input_pos_ = 0;
  std::string output_;
  std::string full_output_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_25::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  std::string collect_and_go_to_security =
      "north\n"
      "take mug\n"
      "north\n"
      "take food ration\n"
      "south\n"
      "east\n"
      "north\n"
      "east\n"
      "take semiconductor\n"
      "west\n"
      "south\n"
      "south\n"
      "west\n"
      "south\n"
      "east\n"
      "take ornament\n"
      "north\n"
      "take coin\n"
      "east\n"
      "take mutex\n"
      "west\n"
      "south\n"
      "east\n"
      "take candy cane\n"
      "west\n"
      "west\n"
      "south\n"
      "east\n"
      "take mouse\n"
      "south\n"
      "inv\n";
  std::vector<std::string> inventory = {
      "mug",  "food ration", "semiconductor", "ornament",
      "coin", "mutex",       "candy cane",    "mouse"};
  for (int i = 0; i < (1 << inventory.size()); ++i) {
    // TODO(@monkeynova): We don't have to re-run from scratch.
    IntCode tmp_codes = codes->Clone();
    std::string command = collect_and_go_to_security;
    for (int j = 0; j < inventory.size(); ++j) {
      if (i & (1 << j)) {
        absl::StrAppend(&command, "drop ", inventory[j], "\n");
      }
    }
    absl::StrAppend(&command, "west\n");
    Terminal t(command);
    absl::Status st = tmp_codes.Run(&t);
    (void)st;  // Input exhausted...
    int code;
    if (RE2::PartialMatch(t.full_output(),
                          "You should be able to get in by typing (\\d+)",
                          &code)) {
      return IntReturn(code);
    }
  }
  return absl::InternalError("Not found");
}

absl::StatusOr<std::string> Day_2019_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
