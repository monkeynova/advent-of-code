// http://adventofcode.com/2023/day/14

#include "advent_of_code/2023/day14/day14.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

void Roll(CharBoard& b, Point dir) {
  std::vector<Point> rocks = b.FindVec('O');
  absl::c_sort(
     rocks,
     [dir](Point a, Point b) {
       if (dir == Cardinal::kNorth) {
         return a.y < b.y;
       } else if (dir == Cardinal::kSouth) {
         return a.y > b.y;
       } else if (dir == Cardinal::kWest) {
         return a.x < b.x;
       } else if (dir == Cardinal::kEast) {
         return a.x > b.x;
       } else {
         LOG(FATAL) << "dir: " << dir;
       }
     });

  for (Point p : rocks) {
    for (Point move = p + dir; 
         b.OnBoard(move) && b[move] == '.';
         move += dir, p += dir) {
      b[move] = 'O';
      b[p] = '.';
    }
  }
}

int CountLoad(const CharBoard& b) {
  std::vector<Point> rocks = b.FindVec('O');
  int load = 0;
  for (Point p : rocks) {
    load += b.height() - p.y;
  }
  return load;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  Roll(b, Cardinal::kNorth);
  VLOG(2) << "Moved:\n" << b;
  return AdventReturn(CountLoad(b));
}

absl::StatusOr<std::string> Day_2023_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  LoopHistory<CharBoard> hist;
  for (int i = 0; i < 1000000000; ++i) {
    if (hist.AddMaybeNew(b)) {
      VLOG(1) << "Loop at " << i;
      return AdventReturn(CountLoad(hist.FindInLoop(1000000000)));
    }
    for (Point dir : {Cardinal::kNorth, Cardinal::kWest, Cardinal::kSouth, Cardinal::kEast}) {
      Roll(b, dir);
    }
  }
  return AdventReturn(CountLoad(b));
}

}  // namespace advent_of_code
