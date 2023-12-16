// http://adventofcode.com/2023/day/16

#include "advent_of_code/2023/day16/day16.h"

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

void FindEnergized(
    const CharBoard& b, Point p, Point d,
    absl::flat_hash_set<std::pair<Point, Point>>* hist,
    absl::flat_hash_set<Point> *energized) {
  for (p += d; b.OnBoard(p); p += d) {
    if (!hist->insert({p, d}).second) break;
    energized->insert(p);
    char c = b[p];
    switch (c) {
      case '.': break;
      case '/': {
        if (d == Cardinal::kNorth) d = Cardinal::kEast;
        else if (d == Cardinal::kSouth) d = Cardinal::kWest;
        else if (d == Cardinal::kEast) d = Cardinal::kNorth;
        else if (d == Cardinal::kWest) d = Cardinal::kSouth;
        else LOG(FATAL) << "Bad dir";
        break;
      }
      case '\\': {
        if (d == Cardinal::kNorth) d = Cardinal::kWest;
        else if (d == Cardinal::kSouth) d = Cardinal::kEast;
        else if (d == Cardinal::kEast) d = Cardinal::kSouth;
        else if (d == Cardinal::kWest) d = Cardinal::kNorth;
        else LOG(FATAL) << "Bad dir";
        break;
      }
      case '-': {
        if (d == Cardinal::kNorth || d == Cardinal::kSouth) {
          FindEnergized(b, p, Cardinal::kEast, hist, energized);
          d = Cardinal::kWest;
        } else if (d == Cardinal::kEast || d == Cardinal::kWest) {
          // Pass trhough.
        } else {
          LOG(FATAL) << "Bad dir";
        }
        break;
      }
      case '|': {
        if (d == Cardinal::kEast || d == Cardinal::kWest) {
          FindEnergized(b, p, Cardinal::kNorth, hist, energized);
          d = Cardinal::kSouth;
        } else if (d == Cardinal::kNorth || d == Cardinal::kSouth) {
          // Pass trhough.
        } else {
          LOG(FATAL) << "Bad dir";
        }
        break;
      }
      default: LOG(FATAL) << "Bad board: " << absl::string_view(&c, 1);
    }
  }
}

}  // namespace

absl::StatusOr<std::string> Day_2023_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> energized;
  absl::flat_hash_set<std::pair<Point, Point>> hist;
  FindEnergized(b, {-1, 0}, Cardinal::kEast, &hist, &energized);
  return AdventReturn(energized.size());
}

absl::StatusOr<std::string> Day_2023_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    {
      absl::flat_hash_set<Point> energized;
      absl::flat_hash_set<std::pair<Point, Point>> hist;
      FindEnergized(b, {x, -1}, Cardinal::kSouth, &hist, &energized);
      max = std::max<int>(max, energized.size());
    }
    {
      absl::flat_hash_set<Point> energized;
      absl::flat_hash_set<std::pair<Point, Point>> hist;
      FindEnergized(b, {x, b.height()}, Cardinal::kNorth, &hist, &energized);
      max = std::max<int>(max, energized.size());
    }
  }
  for (int y = 0; y < b.height(); ++y) {
    {
      absl::flat_hash_set<Point> energized;
      absl::flat_hash_set<std::pair<Point, Point>> hist;
      FindEnergized(b, {-1, y}, Cardinal::kEast, &hist, &energized);
      max = std::max<int>(max, energized.size());
    }
    {
      absl::flat_hash_set<Point> energized;
      absl::flat_hash_set<std::pair<Point, Point>> hist;
      FindEnergized(b, {b.width(), y}, Cardinal::kWest, &hist, &energized);
      max = std::max<int>(max, energized.size());
    }
  }
  return AdventReturn(max);
}

}  // namespace advent_of_code
