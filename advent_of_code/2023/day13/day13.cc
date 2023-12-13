// http://adventofcode.com/2023/day/13

#include "advent_of_code/2023/day13/day13.h"

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

int ScoreReflection(const CharBoard& b) {
  int64_t ret = 0;
  for (int v_test = 0; v_test + 1 < b.width(); ++v_test) {
    bool is_symmetric = true;
    for (Point p : b.range()) {
      if (p.x > v_test) continue;
      Point r = {(v_test - p.x) + v_test + 1, p.y};
      if (!b.OnBoard(r)) continue;
      if (b[p] != b[r]) {
        is_symmetric = false;
        break;
      }
    }
    if (is_symmetric) {
      VLOG(1) << "v_test = " << v_test;
      ret += v_test + 1;
    }
  }
  for (int h_test = 0; h_test + 1 < b.height(); ++h_test) {
    bool is_symmetric = true;
    for (Point p : b.range()) {
      if (p.y > h_test) continue;
      Point r = {p.x, (h_test - p.y) + h_test + 1 };
      if (!b.OnBoard(r)) continue;
      if (b[p] != b[r]) {
        is_symmetric = false;
        break;
      }
    }
    if (is_symmetric) {
      VLOG(1) << "h_test = " << h_test;
      ret += 100 * (h_test + 1);
    }
  }
  VLOG(1) << "score = " << ret;
  if (ret == 0) {
    VLOG(1) << "No Score\n" << b;
  }
  return ret;
}

int ScoreReflectionSmudge(const CharBoard& b) {
  int64_t ret = 0;
  for (int v_test = 0; v_test + 1 < b.width(); ++v_test) {
    std::optional<Point> smudge;
    for (Point p : b.range()) {
      if (p.x > v_test) continue;
      Point r = {(v_test - p.x) + v_test + 1, p.y};
      if (!b.OnBoard(r)) continue;
      if (b[p] != b[r]) {
        if (smudge) {
          smudge = std::nullopt;
          break;
        }
        smudge = p;
      }
    }
    if (smudge) {
      VLOG(1) << "v_test = " << v_test;
      ret += v_test + 1;
    }
  }
  for (int h_test = 0; h_test + 1 < b.height(); ++h_test) {
    std::optional<Point> smudge;
    for (Point p : b.range()) {
      if (p.y > h_test) continue;
      Point r = {p.x, (h_test - p.y) + h_test + 1 };
      if (!b.OnBoard(r)) continue;
      if (b[p] != b[r]) {
        if (smudge) {
          smudge = std::nullopt;
          break;
        }
        smudge = p;
      }
    }
    if (smudge) {
      VLOG(1) << "h_test = " << h_test;
      ret += 100 * (h_test + 1);
    }
  }
  VLOG(1) << "score = " << ret;
  if (ret == 0) {
    VLOG(1) << "No Score\n" << b;
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_13::Part1(
    absl::Span<std::string_view> input) const {
  int last_empty = -1;
  int total = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(last_empty + 1, i - last_empty - 1)));
      total += ScoreReflection(b);
      last_empty = i;
    }
  }
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(last_empty + 1, input.size() - last_empty - 1)));
  total += ScoreReflection(b);
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_13::Part2(
    absl::Span<std::string_view> input) const {
  int last_empty = -1;
  int total = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(last_empty + 1, i - last_empty - 1)));
      total += ScoreReflectionSmudge(b);
      last_empty = i;
    }
  }
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(last_empty + 1, input.size() - last_empty - 1)));
  total += ScoreReflectionSmudge(b);
  return AdventReturn(total);
}

}  // namespace advent_of_code
