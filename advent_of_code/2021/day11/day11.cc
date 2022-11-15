#include "advent_of_code/2021/day11/day11.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int64_t RunStep(CharBoard& b) {
  absl::flat_hash_set<Point> flashed;
  for (Point p : b.range()) {
    ++b[p];
  }
  bool saw_flash = true;
  while (saw_flash) {
    saw_flash = false;
    for (Point p : b.range()) {
      if (flashed.contains(p)) continue;
      if (b[p] > '9') {
        flashed.insert(p);
        saw_flash = true;
        for (Point d : Cardinal::kEightDirs) {
          Point n = d + p;
          if (!b.OnBoard(n)) continue;
          ++b[n];
        }
      }
    }
  }
  for (Point p : flashed) {
    b[p] = '0';
  }
  return flashed.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_11::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  int64_t flashes = 0;
  for (int i = 0; i < 100; ++i) {
    flashes += RunStep(*board);
  }

  return IntReturn(flashes);
}

absl::StatusOr<std::string> Day_2021_11::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  int64_t all_flash = board->width() * board->height();
  for (int i = 1; true; ++i) {
    int64_t flash_count = RunStep(*board);
    if (flash_count == all_flash) {
      return IntReturn(i);
    }
  }

  return Error("No end?");
}

}  // namespace advent_of_code
