#include "advent_of_code/2016/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point_walk.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct KeyState {
  int need = 0;
  int have = 0;

  KeyState(const CharBoard& b) {
    for (Point p : b.range()) {
      if (b[p] > '0' && b[p] <= '9') {
        need |= 1 << (b[p] - '1');
      }
    }
  }

  bool operator==(const KeyState& ks) const {
    return need == ks.need && have == ks.have;
  }
  template <typename H>
  friend H AbslHashValue(H h, const KeyState& ks) {
    return H::combine(std::move(h), ks.need, ks.have);
  }
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const KeyState& ks) {
    absl::Format(&sink, "0x%x/0x%x", ks.have, ks.need);
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2016_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::flat_hash_set<Point> start_set = b->Find('0');
  if (start_set.size() != 1) return Error("No unique start");
  Point start = *start_set.begin();

  return AdventReturn(PointWalkData<KeyState>({
    .start = {.p = start, .d = KeyState(*b)},
    .is_good = [&](PointAndData<KeyState>& cur, int) {
      if (!b->OnBoard(cur.p)) return false;
      char board_char = (*b)[cur.p];
      if (board_char == '#') return false;
      if (board_char > '0' && board_char <= '9') {
        cur.d.have |= 1 << (board_char - '1');
      }
      return true;
    },
    .is_final = [&](PointAndData<KeyState> cur, int) {
      return cur.d.need == cur.d.have;
    },
  }).FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  absl::flat_hash_set<Point> start_set = b->Find('0');
  if (start_set.size() != 1) return Error("No unique start");
  Point start = *start_set.begin();

  return AdventReturn(PointWalkData<KeyState>({
    .start = {.p = start, .d = KeyState(*b)},
    .is_good = [&](PointAndData<KeyState>& cur, int) {
      if (!b->OnBoard(cur.p)) return false;
      char board_char = (*b)[cur.p];
      if (board_char == '#') return false;
      if (board_char > '0' && board_char <= '9') {
        cur.d.have |= 1 << (board_char - '1');
      }
      return true;
    },
    .is_final = [&](PointAndData<KeyState> cur, int) {
      return cur.d.need == cur.d.have && cur.p == start;
    },
  }).FindMinSteps());
}

}  // namespace advent_of_code
