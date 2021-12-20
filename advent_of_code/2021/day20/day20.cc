#include "advent_of_code/2021/day20/day20.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Conway {
 public:
  Conway(CharBoard b, std::string lookup, char fill = '.')
   : b_(std::move(b)), lookup_(std::move(lookup)), fill_(fill) {}

 const CharBoard& board() const { return b_; }
 char fill() const { return fill_; }

 absl::Status Advance();
 absl::Status AdvanceN(int count) {
   for (int i = 0; i < count; ++i) {
     if (auto st = Advance(); !st.ok()) return st;
   }
   return absl::OkStatus();
 }

 private:
  CharBoard b_;
  std::string lookup_;
  char fill_;
};

absl::Status Conway::Advance() {
  CharBoard new_b(b_.width() + 2, b_.height() + 2);
  for (Point p : new_b.range()) {
    int bv = 0;
    for (Point d :
         {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
          Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
          Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast}) {
      Point src = p + Cardinal::kNorthWest + d;
      bv *= 2;
      char src_val = b_.OnBoard(src) ? b_[src] : fill_;
      if (src_val == '#') {
        ++bv;
      } else if (src_val != '.') {
        return Error("Bad board");
      }
    }
    if (bv >= lookup_.size()) return Error("Bad lookup size");
    new_b[p] = lookup_[bv];
  }
  b_ = std::move(new_b);
  if (fill_ == '.') {
    fill_ = lookup_[0];
  } else {
    if (fill_ != '#') return Error("Bad board");
    fill_ = lookup_[511];
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();

  Conway conway(*b, std::string(lookup));
  if (auto st = conway.AdvanceN(2); !st.ok()) return st;
  if (conway.fill() != '.') {
    return absl::UnimplementedError("Can't return a board with non-'.' fill");
  }

  return IntReturn(conway.board().CountOn());
}

absl::StatusOr<std::string> Day_2021_20::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();

  Conway conway(*b, std::string(lookup));
  if (auto st = conway.AdvanceN(50); !st.ok()) return st;
  if (conway.fill() != '.') {
    return absl::UnimplementedError("Can't return a board with non-'.' fill");
  }

  return IntReturn(conway.board().CountOn());
}

}  // namespace advent_of_code
