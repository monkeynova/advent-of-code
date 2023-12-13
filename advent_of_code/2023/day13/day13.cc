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

int ScoreReflection(const ImmutableCharBoard& b, int diff_test = 0) {
  for (int v_test = 0; v_test + 1 < b.width(); ++v_test) {
    int diff = 0;
    for (int y = 0; y < b.height(); ++y) {
      std::string_view row = b.row(y);
      for (int x = v_test; x >= 0; --x) {
        int test_x = 2 * v_test - x + 1;
        if (test_x >= b.width()) break;
        if (row[x] != row[test_x]) {
          if (++diff > diff_test) {
            break;
          }
        }
      }
      if (diff > diff_test) break;
    }
    if (diff == diff_test) {
      return v_test + 1;
    }
  }
  for (int h_test = 0; h_test + 1 < b.height(); ++h_test) {
    int diff = 0;
    for (int y = h_test; y >= 0; --y) {
      int test_y = 2 * h_test - y + 1;
      if (test_y >= b.height()) break;
      std::string_view row1 = b.row(y);
      std::string_view row2 = b.row(test_y);
      for (int x = 0; x < b.width(); ++x) {
        if (row1[x] != row2[x]) {
          if (++diff > diff_test) {
            break;
          }
        }
      }
      if (diff > diff_test) {
        break;
      }
    }
    if (diff == diff_test) {
      return 100 * (h_test + 1);
    }
  }
  return 0;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_13::Part1(
    absl::Span<std::string_view> input) const {
  int last_empty = -1;
  int total = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      absl::Span<std::string_view> board =
          input.subspan(last_empty + 1, i - last_empty - 1);
      ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(board));
      total += ScoreReflection(b);
      last_empty = i;
    }
  }
  absl::Span<std::string_view> board =
      input.subspan(last_empty + 1, input.size() - last_empty - 1);
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(board));
  total += ScoreReflection(b);
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_13::Part2(
    absl::Span<std::string_view> input) const {
  int last_empty = -1;
  int total = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      absl::Span<std::string_view> board =
          input.subspan(last_empty + 1, i - last_empty - 1);
      ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(board));
      total += ScoreReflection(b, 1);
      last_empty = i;
    }
  }
  absl::Span<std::string_view> board =
      input.subspan(last_empty + 1, input.size() - last_empty - 1);
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(board));
  total += ScoreReflection(b, 1);
  return AdventReturn(total);
}

}  // namespace advent_of_code
