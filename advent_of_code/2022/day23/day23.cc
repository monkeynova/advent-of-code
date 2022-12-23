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
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool Move(absl::flat_hash_set<Point>& elves, int turn) {
  std::array<Point, 4> checks = {
    Cardinal::kNorth, Cardinal::kSouth, Cardinal::kWest, Cardinal::kEast};

  absl::flat_hash_map<Point, std::vector<Point>> proposals;
  for (Point p : elves) {
    bool any_elf = false;
    for (Point d : Cardinal::kEightDirs) {
      if (elves.contains(p + d)) {
        any_elf = true;
        break;
      }
    }
    if (!any_elf) continue;
    bool proposal_added = false;
    for (int i = 0; i < 4; ++i) {
      Point move = checks[(i + turn) % 4];
      bool any_elf = false;
      for (Point test : {move, move + move.rotate_left(), move + move.rotate_right()}) {
        if (elves.contains(p + test)) {
          any_elf = true;
          break;
        }
      }
      if (any_elf) continue;
      proposals[p + move].push_back(p);
      proposal_added = true;
      break;
    }
    //CHECK(proposal_added);
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

CharBoard Draw(absl::flat_hash_set<Point> elves) {
  PointRectangle pr{*elves.begin(), *elves.begin()};
  for (Point p : elves) {
    pr.ExpandInclude(p);
  }
  CharBoard draw(pr);
  for (Point p : elves) draw[p - pr.min] = '#';
  return draw;
}

int64_t Score(absl::flat_hash_set<Point> elves) {
  PointRectangle pr{*elves.begin(), *elves.begin()};
  for (Point p : elves) {
    pr.ExpandInclude(p);
  }
  return pr.Area() - elves.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2022_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();
  absl::flat_hash_set<Point> elves;
  for (Point p : board->range()) {
    if ((*board)[p] == '#') elves.insert(p);
  }
  for (int i = 0; i < 10; ++i) {
    VLOG(2) << "Board:\n" <<Draw(elves);
    Move(elves, i);
  }
  return IntReturn(Score(elves));
}

absl::StatusOr<std::string> Day_2022_23::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();
  absl::flat_hash_set<Point> elves;
  for (Point p : board->range()) {
    if ((*board)[p] == '#') elves.insert(p);
  }
  for (int i = 0; true; ++i) {
    VLOG(2) << "Board:\n" <<Draw(elves);
    if (!Move(elves, i)) {
      return IntReturn(i + 1);
    }
  }
  return IntReturn(Score(elves));
}

}  // namespace advent_of_code
