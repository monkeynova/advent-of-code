#include "advent_of_code/2019/day2/day2.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

void CorrectBoard(std::vector<int>* codes, int noun, int verb) {
  (*codes)[1] = noun;
  (*codes)[2] = verb;
}

absl::StatusOr<std::vector<std::string>> Day2_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int>> codes = ParseIntcode(input);
  if (!codes.ok()) return codes.status();

  CorrectBoard(&*codes, 12, 2);
  if (absl::Status st = RunIntcode(&*codes); !st.ok()) return st;

  return std::vector<std::string>{absl::StrCat((*codes)[0])};
}

absl::StatusOr<std::vector<std::string>> Day2_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int>> start_codes = ParseIntcode(input);
  if (!start_codes.ok()) return start_codes.status();

  for (int noun = 0; noun < 100; ++noun) {
    for (int verb = 0; verb < 100; ++verb) {
      std::vector<int> codes = *start_codes;
      CorrectBoard(&codes, noun, verb);
      if (absl::Status st = RunIntcode(&codes); !st.ok()) break;
      if (codes[0] == 19690720) {
        return std::vector<std::string>{absl::StrCat(100 * noun + verb)};
      }
    }
  }
  return std::vector<std::string>{""};
}
