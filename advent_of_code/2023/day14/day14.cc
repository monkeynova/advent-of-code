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

void RollNorth(CharBoard& b) {
  for (int x = 0; x < b.width(); ++x) {
    int dest = 0;
    for (int y = 0; y < b.height(); ++y) {
      switch (b[{x,y}]) {
        case 'O': {
          if (dest != y) {        
            b[{x, dest}] = 'O';
            b[{x, y}] = '.';
          }
          ++dest;
          break;
        }
        case '#': {
          dest = y + 1;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollSouth(CharBoard& b) {
  for (int x = 0; x < b.width(); ++x) {
    int dest = b.height() - 1;
    for (int y = b.height() - 1; y >= 0; --y) {
      switch (b[{x,y}]) {
        case 'O': {
          if (dest != y) {        
            b[{x, dest}] = 'O';
            b[{x, y}] = '.';
          }
          --dest;
          break;
        }
        case '#': {
          dest = y - 1;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollWest(CharBoard& b) {
  for (int y = 0; y < b.height(); ++y) {
    int dest = 0;
    for (int x = 0; x < b.width(); ++x) {
      switch (b[{x,y}]) {
        case 'O': {
          if (dest != x) {        
            b[{dest, y}] = 'O';
            b[{x, y}] = '.';
          }
          ++dest;
          break;
        }
        case '#': {
          dest = x + 1;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollEast(CharBoard& b) {
  for (int y = 0; y < b.height(); ++y) {
    int dest = b.width() - 1;
    for (int x = b.width() - 1; x >= 0; --x) {
      switch (b[{x,y}]) {
        case 'O': {
          if (dest != x) {        
            b[{dest, y}] = 'O';
            b[{x, y}] = '.';
          }
          --dest;
          break;
        }
        case '#': {
          dest = x - 1;
          break;
        }
        case '.': {
          break;
        }
      }
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
  RollNorth(b);
  VLOG(2) << "Moved:\n" << b;
  return AdventReturn(CountLoad(b));
}

absl::StatusOr<std::string> Day_2023_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  LoopHistory<CharBoard> hist;
  for (int i = 0; i < 1000000000; ++i) {
    if (hist.AddMaybeNew(b)) {
      VLOG(1) << hist;
      return AdventReturn(CountLoad(hist.FindInLoop(1000000000)));
    }
    RollNorth(b);
    RollWest(b);
    RollSouth(b);
    RollEast(b);
  }
  return AdventReturn(CountLoad(b));
}

}  // namespace advent_of_code
