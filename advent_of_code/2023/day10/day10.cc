// http://adventofcode.com/2023/day/10

#include "advent_of_code/2023/day10/day10.h"

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

char StartType(Point orig_dir, Point dir) {
  if      (orig_dir == Cardinal::kEast  && dir == Cardinal::kEast ) return '-';
  else if (orig_dir == Cardinal::kWest  && dir == Cardinal::kWest ) return '-';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kSouth) return '|';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kNorth) return '|';
  else if (orig_dir == Cardinal::kWest  && dir == Cardinal::kNorth) return '7';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kEast ) return '7';
  else if (orig_dir == Cardinal::kWest  && dir == Cardinal::kSouth) return 'J';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kEast ) return 'J';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kWest ) return 'L';
  else if (orig_dir == Cardinal::kEast  && dir == Cardinal::kSouth) return 'L';
  else if (orig_dir == Cardinal::kEast  && dir == Cardinal::kNorth) return 'F';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kWest ) return 'F';
  return '?';
}

std::vector<Point> FindLoop(const CharBoard& b, Point start, char* start_pipe_dest = nullptr) {
  for (Point dir : Cardinal::kFourDirs) {
    Point orig_dir = dir;
    std::vector<Point> ret; 
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      ret.push_back(test);
      if (test == start) {
        if (start_pipe_dest != nullptr) {
          *start_pipe_dest = StartType(orig_dir, dir);
        }
        return ret;
      }
      bool off_loop = false;
      switch (b[test]) {
        case '|': {
          if (dir != Cardinal::kNorth && dir != Cardinal::kSouth) {
            off_loop = true;
          }
          break;
        }
        case '-': {
          if (dir != Cardinal::kEast && dir != Cardinal::kWest) {
            off_loop = true;
          }
          break;
        }
        case 'F': {
          if (dir == Cardinal::kNorth) dir = Cardinal::kEast;
          else if (dir == Cardinal::kWest) dir = Cardinal::kSouth;
          else off_loop = true;
          break;
        }
        case '7': {
          if (dir == Cardinal::kNorth) dir = Cardinal::kWest;
          else if (dir == Cardinal::kEast) dir = Cardinal::kSouth;
          else off_loop = true;
          break;
        }
        case 'J': {
          if (dir == Cardinal::kSouth) dir = Cardinal::kWest;
          else if (dir == Cardinal::kEast) dir = Cardinal::kNorth;
          else off_loop = true;
          break;
        }
        case 'L': {
          if (dir == Cardinal::kSouth) dir = Cardinal::kEast;
          else if (dir == Cardinal::kWest) dir = Cardinal::kNorth;
          else off_loop = true;
          break;
        }
        default: {
          off_loop = true;
          break;
        }
      }
      if (off_loop) break;
    }
  }
  return {};
}

absl::StatusOr<int> InsideSpace(const CharBoard& b, std::vector<Point> loop, char start_pipe) {
  Point start = loop.back();
  absl::c_sort(loop, PointYThenXLT{});

  int count_inside = 0;
  bool upper_inside = false;
  bool lower_inside = false;
  bool prev_inside = false;
  for (int i = 0; i < loop.size(); ++i) {
    Point p = loop[i];
    char c = p == start ? start_pipe : b[p];
    switch (c) {
      case '|': {
        if (upper_inside != lower_inside) return Error("Bad |");
        upper_inside = !upper_inside;
        lower_inside = !lower_inside;
        break;
      }
      case 'J': {
        if (upper_inside == lower_inside) return Error("Bad J");
        upper_inside = lower_inside;
        break;
      }
      case '7': {
        if (upper_inside == lower_inside) return Error("Bad 7");
        lower_inside = upper_inside;
        break;
      }
      case 'L': {
        if (upper_inside != lower_inside) return Error("Bad L");
        upper_inside = !upper_inside;
        break;
      }
      case 'F': {
        if (upper_inside != lower_inside) return Error("Bad F");
        lower_inside = !lower_inside;
        break;
      }
      case '-': {
        if (upper_inside == lower_inside) return Error("Bad - ");
        break;
      }
      default: {
        return Error("Unknown pipe");
      }
    }
    if (!upper_inside || !lower_inside) {
      if (prev_inside) {
        Point prev = loop[i - 1];
        if (prev.y != p.y) return Error("Bad line");
        count_inside += p.x - prev.x - 1;
      }
      prev_inside = false;
    } else {
      prev_inside = true;
    }
  }
  return count_inside;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_10::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> start_set = b.Find('S');
  if (start_set.size() != 1) return Error("Bad start");
  Point start = *start_set.begin();
  std::vector<Point> loop = FindLoop(b, start);
  if (loop.empty()) return Error("No loop found");
  return AdventReturn((loop.size() + 1) / 2);
}

absl::StatusOr<std::string> Day_2023_10::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> start_set = b.Find('S');
  if (start_set.size() != 1) return Error("Bad start");
  Point start = *start_set.begin();
  char start_pipe = '?';
  std::vector<Point> loop = FindLoop(b, start, &start_pipe);
  if (loop.empty()) return Error("No loop found");
  if (start_pipe == '?') return Error("Bad start pipe");
  return AdventReturn(InsideSpace(b, std::move(loop), start_pipe));
}

}  // namespace advent_of_code
