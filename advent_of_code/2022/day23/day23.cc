// http://adventofcode.com/2022/day/23

#include "advent_of_code/2022/day23/day23.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool Move(absl::flat_hash_set<Point>& elves, int turn) {
  constexpr std::array<Point, 4> kDirProposeOrder = {
      Cardinal::kNorth, Cardinal::kSouth, Cardinal::kWest, Cardinal::kEast};
  constexpr std::array<std::array<Point, 3>, 4> kDirTestOrder = {
      std::array<Point, 3>{Cardinal::kNorth, Cardinal::kNorthWest,
                           Cardinal::kNorthEast},
      std::array<Point, 3>{Cardinal::kSouth, Cardinal::kSouthWest,
                           Cardinal::kSouthEast},
      std::array<Point, 3>{Cardinal::kWest, Cardinal::kNorthWest,
                           Cardinal::kSouthWest},
      std::array<Point, 3>{Cardinal::kEast, Cardinal::kNorthEast,
                           Cardinal::kSouthEast},
  };

  absl::flat_hash_map<Point, std::vector<Point>> proposals;
  for (Point p : elves) {
    auto has_elf = [&elves, p](Point d) { return elves.contains(p + d); };

    if (absl::c_none_of(Cardinal::kEightDirs, has_elf)) continue;

    for (int i = 0; i < 4; ++i) {
      int proposal = (i + turn) % 4;
      Point move = kDirProposeOrder[proposal];
      if (absl::c_any_of(kDirTestOrder[proposal], has_elf)) continue;
      proposals[p + move].push_back(p);
      break;
    }
  }
  bool moved = false;
  for (const auto& [to, from_set] : proposals) {
    if (from_set.size() != 1) continue;
    elves.erase(from_set[0]);
    elves.insert(to);
    moved = true;
  }
  return moved;
}

int64_t Score(absl::flat_hash_set<Point> elves) {
  return PointRectangle::Bounding(elves).Area() - elves.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2022_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  absl::flat_hash_set<Point> elves = board.Find('#');

  for (int i = 0; i < 10; ++i) {
    VLOG(2) << "Board:\n" << CharBoard::DrawNew(elves);
    Move(elves, i);
  }

  return AdventReturn(Score(elves));
}

absl::StatusOr<std::string> Day_2022_23::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard board, CharBoard::Parse(input));

  absl::flat_hash_set<Point> elves = board.Find('#');

  for (int i = 0; true; ++i) {
    VLOG(2) << "Board:\n" << CharBoard::DrawNew(elves);
    if (!Move(elves, i)) {
      return AdventReturn(i + 1);
    }
  }

  LOG(FATAL) << "Left infinite loop";
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/23, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_23());
});

}  // namespace advent_of_code
