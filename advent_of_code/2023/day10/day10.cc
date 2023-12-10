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

std::vector<Point> FindLoop(const CharBoard& b) {
  absl::flat_hash_set<Point> start_set = b.Find('S');
  if (start_set.size() != 1) return {};

  Point start = *start_set.begin();
  for (Point dir : Cardinal::kFourDirs) {
    std::vector<Point> ret; 
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      ret.push_back(test);
      if (test == start) {
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

}  // namespace

absl::StatusOr<std::string> Day_2023_10::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  std::vector<Point> loop = FindLoop(b);
  if (loop.empty()) return Error("No loop found");
  return AdventReturn((loop.size() + 1) / 2);
}

absl::StatusOr<std::string> Day_2023_10::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  absl::flat_hash_set<Point> start_set = b.Find('S');
  if (start_set.size() != 1) return Error("Bad start");
  Point start = *start_set.begin();
  for (Point dir : Cardinal::kFourDirs) {
    Point orig_dir = dir;
    absl::flat_hash_set<Point> loop;
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      loop.insert(test);
      if (test == start) {
        char start_pipe = '?';
        if (orig_dir == Cardinal::kEast && dir == Cardinal::kEast) start_pipe = '-';
        else if (orig_dir == Cardinal::kWest && dir == Cardinal::kWest) start_pipe = '-';
        else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kSouth) start_pipe = '|';
        else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kNorth) start_pipe = '|';
        else if (orig_dir == Cardinal::kWest && dir == Cardinal::kNorth) start_pipe = '7';
        else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kEast) start_pipe = '7';
        else if (orig_dir == Cardinal::kWest && dir == Cardinal::kSouth) start_pipe = 'J';
        else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kEast) start_pipe = 'J';
        else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kWest) start_pipe = 'L';
        else if (orig_dir == Cardinal::kEast && dir == Cardinal::kSouth) start_pipe = 'L';
        else if (orig_dir == Cardinal::kEast && dir == Cardinal::kNorth) start_pipe = 'F';
        else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kWest) start_pipe = 'F';

        VLOG(1) << "orig=" << orig_dir << "; dir=" << dir;
        VLOG(1) << "S=" << std::string_view(&start_pipe, 1);
        if (start_pipe == '?') return Error("Bad start pipe");

        bool upper_inside = false;
        bool lower_inside = false;
        int count_inside = 0;
        VLOG(1) << CharBoard::DrawNew(loop);
        CharBoard inside_draw = b;
        for (const auto [p, c] : b) {
          if (p.x == 0 && upper_inside) return Error("Parity failure (upper)");
          if (p.x == 0 && lower_inside) return Error("Parity failure (lower)");
          VLOG(1) << p << "; " << upper_inside << "/" << lower_inside;
          if (loop.contains(p)) {
            char test_c = p == start ? start_pipe : c;
            VLOG(1) << "test_c = " << std::string_view(&test_c, 1);
            switch (test_c) {
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
                if (upper_inside != lower_inside) return Error("Bad L");
                lower_inside = !lower_inside;
                break;
              }
            }
          } else if (upper_inside != lower_inside) {
            if (c != '-') return Error("Bad -");
          } else if (upper_inside) {
            ++count_inside;
            inside_draw[p] = 'I';
          }
        }
        VLOG(1) << inside_draw;
        return AdventReturn(count_inside);
      }
      if (b[test] == '.') break;
      if (b[test] == '|')  {
        if (dir != Cardinal::kNorth && dir != Cardinal::kSouth) break;
      }
      if (b[test] == '-')  {
        if (dir != Cardinal::kEast && dir != Cardinal::kWest) break;
      }
      if (b[test] == 'F')  {
        if (dir == Cardinal::kNorth) dir = Cardinal::kEast;
        else if (dir == Cardinal::kWest) dir = Cardinal::kSouth;
        else break;
      }
      if (b[test] == '7')  {
        if (dir == Cardinal::kNorth) dir = Cardinal::kWest;
        else if (dir == Cardinal::kEast) dir = Cardinal::kSouth;
        else break;
      }
      if (b[test] == 'J')  {
        if (dir == Cardinal::kSouth) dir = Cardinal::kWest;
        else if (dir == Cardinal::kEast) dir = Cardinal::kNorth;
        else break;
      }
      if (b[test] == 'L')  {
        if (dir == Cardinal::kSouth) dir = Cardinal::kEast;
        else if (dir == Cardinal::kWest) dir = Cardinal::kNorth;
        else break;
      }
    }
  }
  return Error("No loop found");
}

}  // namespace advent_of_code
