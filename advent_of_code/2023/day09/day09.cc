// http://adventofcode.com/2023/day/9

#include "advent_of_code/2023/day09/day09.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2023_09::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(
      std::vector<int64_t> vals,
      ParseAsInts(absl::StrSplit(line, " ")));
    std::vector<int64_t> end_vals;
    for (bool done = false; !done;) {
      end_vals.push_back(vals.back());
      done = true;
      for (int i = 0; i + 1 < vals.size(); ++i) {
        vals[i] = vals[i + 1] - vals[i];
        if (vals[i] != 0) done = false;
      }
      vals.pop_back();
    }
    int64_t predict = 0;
    while (!end_vals.empty()) {
      predict += end_vals.back();
      end_vals.pop_back();
    }
    total += predict;
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_09::Part2(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(
      std::vector<int64_t> vals,
      ParseAsInts(absl::StrSplit(line, " ")));
    std::vector<int64_t> begin_vals;
    for (bool done = false; !done;) {
      begin_vals.push_back(vals[0]);
      done = true;
      for (int i = 0; i + 1 < vals.size(); ++i) {
        vals[i] = vals[i + 1] - vals[i];
        if (vals[i] != 0) done = false;
      }
      vals.pop_back();
    }
    int64_t predict = 0;
    while (!begin_vals.empty()) {
      predict = begin_vals.back() - predict;
      begin_vals.pop_back();
    }
    total += predict;
  }
  return AdventReturn(total);
}

}  // namespace advent_of_code
