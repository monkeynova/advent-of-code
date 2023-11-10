// http://adventofcode.com/2019/day/21

#include "advent_of_code/2019/day21/day21.h"

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

class SpringBot : public IntCode::IOModule {
 public:
  SpringBot(std::string_view program) : in_buf_(program) {}

  int64_t damage() const { return damage_; }

  bool PauseIntCode() override { return false; }

  absl::StatusOr<int64_t> Fetch() override {
    if (in_buf_pos_ < in_buf_.size()) {
      return in_buf_[in_buf_pos_++];
    }
    return absl::InternalError("No more input");
  }

  absl::Status Put(int64_t val) override {
    if (val >= 128) {
      damage_ = val;
      LOG(WARNING) << "Damage: " << damage_;
    } else if (val == '\n') {
      LOG(WARNING) << "Output: " << cur_line_;
      out_buf_.push_back(std::move(cur_line_));
      cur_line_.clear();
    } else {
      cur_line_.append(1, val);
    }
    return absl::OkStatus();
  }

 private:
  std::string in_buf_;
  int in_buf_pos_ = 0;
  std::vector<std::string> out_buf_;
  std::string cur_line_;
  int64_t damage_ = -1;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_21::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  // JUMP goes 4 spaces forward.
  // Jump = D & (!A | !B | !C)

  SpringBot spring_bot(
      "NOT A J\n"
      "NOT B T\n"
      "OR T J\n"
      "NOT C T\n"
      "OR T J\n"
      "AND D J\n"
      "WALK\n");
  if (absl::Status st = codes->Run(&spring_bot); !st.ok()) return st;

  return AdventReturn(spring_bot.damage());
}

absl::StatusOr<std::string> Day_2019_21::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  // JUMP goes 4 spaces forward.
  // Jump = D & (!A | !B | !C) & !(!E & !H)
  // Jump = D & (!A | !B | !C) & (E | H)
  //

  SpringBot spring_bot(
      "NOT A J\n"
      "NOT B T\n"
      "OR T J\n"
      "NOT C T\n"
      "OR T J\n"
      "NOT H T\n"
      "NOT T T\n"
      "OR E T\n"
      "AND T J\n"
      "AND D J\n"
      "RUN\n");
  if (absl::Status st = codes->Run(&spring_bot); !st.ok()) return st;

  return AdventReturn(spring_bot.damage());
}

}  // namespace advent_of_code
