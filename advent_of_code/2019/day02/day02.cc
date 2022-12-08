#include "advent_of_code/2019/day02/day02.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"

namespace advent_of_code {
namespace {

absl::Status CorrectBoard(IntCode* codes, int noun, int verb) {
  if (absl::Status st = codes->Poke(1, noun); !st.ok()) return st;
  if (absl::Status st = codes->Poke(2, verb); !st.ok()) return st;
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2019_02::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  if (absl::Status st = CorrectBoard(&*codes, 12, 2); !st.ok()) return st;
  if (absl::Status st = codes->Run(); !st.ok()) return st;

  return IntReturn(codes->Peek(0));
}

absl::StatusOr<std::string> Day_2019_02::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> start_codes = IntCode::Parse(input);
  if (!start_codes.ok()) return start_codes.status();

  for (int noun = 0; noun < 100; ++noun) {
    for (int verb = 0; verb < 100; ++verb) {
      IntCode codes = start_codes->Clone();
      if (absl::Status st = CorrectBoard(&codes, noun, verb); !st.ok())
        return st;
      if (absl::Status st = codes.Run(); !st.ok()) break;
      absl::StatusOr<int> val = codes.Peek(0);
      if (!val.ok()) return val.status();
      if (*val == 19690720) {
        return IntReturn(100 * noun + verb);
      }
    }
  }
  return absl::NotFoundError("Culd not find value");
}

}  // namespace advent_of_code
