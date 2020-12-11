#include "advent_of_code/2019/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"
#include "re2/re2.h"

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

absl::StatusOr<std::vector<std::string>> Day25_2019::Part1(
    const std::vector<absl::string_view>& input) const {
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
    "mug", "food ration", "semiconductor", "ornament", "coin", "mutex",
    "candy cane", "mouse"
  };
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
    (void)st; // Input exhausted...
    int code;
    if (RE2::PartialMatch(t.full_output(), "You should be able to get in by typing (\\d+)", &code)) {
      return IntReturn(code);
    }
  }
  return absl::InternalError("Not found");
}

absl::StatusOr<std::vector<std::string>> Day25_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>({"Merry Christmas"});
}
