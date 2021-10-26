#include "advent_of_code/2020/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Seat {
  int col;
  int row;
  int id() const { return col * 8 + row; }
};

absl::StatusOr<Seat> ParseSeat(absl::string_view seat_str) {
  Seat seat{0, 0};
  if (seat_str.size() != 10) return absl::InvalidArgumentError("");
  for (int i = 0; i < 7; ++i) {
    if (seat_str[i] == 'F') {
      seat.col <<= 1;
    } else if (seat_str[i] == 'B') {
      seat.col = (seat.col << 1) | 1;
    } else {
      return absl::InvalidArgumentError("");
    }
  }
  for (int i = 7; i < 10; ++i) {
    if (seat_str[i] == 'L') {
      seat.row <<= 1;
    } else if (seat_str[i] == 'R') {
      seat.row = (seat.row << 1) | 1;
    } else {
      return absl::InvalidArgumentError("");
    }
  }
  return seat;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_05::Part1(
    absl::Span<absl::string_view> input) const {
  int maxid = 0;
  for (absl::string_view rec : input) {
    absl::StatusOr<Seat> seat = ParseSeat(rec);
    if (!seat.ok()) return seat.status();
    maxid = std::max(maxid, seat->id());
  }
  return IntReturn(maxid);
}

absl::StatusOr<std::string> Day_2020_05::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<bool> present;
  present.resize(1 << 10);
  for (absl::string_view rec : input) {
    absl::StatusOr<Seat> seat = ParseSeat(rec);
    if (!seat.ok()) return seat.status();
    present[seat->id()] = true;
  }
  int missingid = -1;
  for (int id = 1; id < present.size() - 1; ++id) {
    if (present[id - 1] && present[id + 1] && !present[id]) {
      missingid = id;
    }
  }
  return IntReturn(missingid);
}

}  // namespace advent_of_code
