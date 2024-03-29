#include "advent_of_code/2020/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Seat {
  int col;
  int row;
  int id() const { return col * 8 + row; }
};

absl::StatusOr<Seat> ParseSeat(std::string_view seat_str) {
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
    absl::Span<std::string_view> input) const {
  int maxid = 0;
  for (std::string_view rec : input) {
    ASSIGN_OR_RETURN(Seat seat, ParseSeat(rec));
    maxid = std::max(maxid, seat.id());
  }
  return AdventReturn(maxid);
}

absl::StatusOr<std::string> Day_2020_05::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<bool> present;
  present.resize(1 << 10);
  for (std::string_view rec : input) {
    ASSIGN_OR_RETURN(Seat seat, ParseSeat(rec));
    present[seat.id()] = true;
  }
  int missingid = -1;
  for (int id = 1; id < present.size() - 1; ++id) {
    if (present[id - 1] && present[id + 1] && !present[id]) {
      missingid = id;
    }
  }
  return AdventReturn(missingid);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_05()); });

}  // namespace advent_of_code
