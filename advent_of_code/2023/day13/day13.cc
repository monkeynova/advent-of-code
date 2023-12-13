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

int ScoreReflection(const ImmutableCharBoard& b) {
  int64_t ret = 0;
  for (int v_test = 0; v_test + 1 < b.width(); ++v_test) {
    bool is_symmetric = true;
    for (int y = 0; y < b.height(); ++y) {
      std::string_view row = b.row(y);
      for (int x = v_test; x >= 0; --x) {
        int test_x = 2 * v_test - x + 1;
        if (test_x >= b.width()) break;
        if (row[x] != row[test_x]) {
          is_symmetric = false;
          break;
        }
      }
      if (!is_symmetric) break;
    }
    if (is_symmetric) {
      VLOG(1) << "v_test = " << v_test;
      ret += v_test + 1;
    }
  }
  for (int h_test = 0; h_test + 1 < b.height(); ++h_test) {
    bool is_symmetric = true;
    for (int y = h_test; y >= 0; --y) {
      int test_y = 2 * h_test - y + 1;
      if (test_y >= b.height()) break;
      if (b.row(y) != b.row(test_y)) {
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

int ScoreReflectionSmudge(const ImmutableCharBoard& b) {
  int64_t ret = 0;
  for (int v_test = 0; v_test + 1 < b.width(); ++v_test) {
    int diff = 0;;
    for (int y = 0; y < b.height(); ++y) {
      std::string_view row = b.row(y);
      for (int x = v_test; x >= 0; --x) {
        int test_x = 2 * v_test - x + 1;
        if (test_x >= b.width()) break;
        if (row[x] != row[test_x]) {
          if (++diff > 1) {
            break;
          }
        }
      }
      if (diff > 1) break;
    }
    if (diff == 1) {
      VLOG(1) << "v_test = " << v_test;
      ret += v_test + 1;
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
          if (++diff > 1) {
            break;
          }
        }
      }
      if (diff > 1) {
        break;
      }
    }
    if (diff == 1) {
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
      ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input.subspan(last_empty + 1, i - last_empty - 1)));
      total += ScoreReflection(b);
      last_empty = i;
    }
  }
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input.subspan(last_empty + 1, input.size() - last_empty - 1)));
  total += ScoreReflection(b);
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_13::Part2(
    absl::Span<std::string_view> input) const {
  int last_empty = -1;
  int total = 0;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input.subspan(last_empty + 1, i - last_empty - 1)));
      total += ScoreReflectionSmudge(b);
      last_empty = i;
    }
  }
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input.subspan(last_empty + 1, input.size() - last_empty - 1)));
  total += ScoreReflectionSmudge(b);
  return AdventReturn(total);
}

}  // namespace advent_of_code
