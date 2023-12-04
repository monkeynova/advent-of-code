// http://adventofcode.com/2023/day/4

#include "advent_of_code/2023/day04/day04.h"

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
#include "re2/re2.h"

namespace advent_of_code {

namespace {
}  // namespace

absl::StatusOr<std::string> Day_2023_04::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    auto [card, rest] = PairSplit(line, ": ");
    auto [win_nums, my_nums] = PairSplit(rest, " | ");
    ASSIGN_OR_RETURN(
      std::vector<int64_t> wins, ParseAsInts(absl::StrSplit(win_nums, " ")));
    absl::flat_hash_set<int64_t> win_set(wins.begin(), wins.end());
    ASSIGN_OR_RETURN(
      std::vector<int64_t> mine, ParseAsInts(absl::StrSplit(my_nums, " ")));
    int64_t win_count = absl::c_accumulate(mine, int64_t{0}, [&](int64_t a, int64_t n) {
      return a + (win_set.contains(n) ? 1 : 0);
    });
    if (win_count > 0) {
      total += 1 << (win_count - 1);
    }
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_04::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<int64_t> counts(input.size(), 1);

  int card_idx = 0;
  for (std::string_view line : input) {
    auto [card, rest] = PairSplit(line, ": ");
    auto [win_nums, my_nums] = PairSplit(rest, " | ");
    ASSIGN_OR_RETURN(
      std::vector<int64_t> wins, ParseAsInts(absl::StrSplit(win_nums, " ")));
    absl::flat_hash_set<int64_t> win_set(wins.begin(), wins.end());
    ASSIGN_OR_RETURN(
      std::vector<int64_t> mine, ParseAsInts(absl::StrSplit(my_nums, " ")));
    int64_t win_count = absl::c_accumulate(mine, int64_t{0}, [&](int64_t a, int64_t n) {
      return a + (win_set.contains(n) ? 1 : 0);
    });
    if (win_count > 0) {
      for (int i = 0; card_idx + 1 + i < counts.size() && i < win_count; ++i) {
        counts[card_idx + 1 + i] += counts[card_idx];
      }
    }
    ++card_idx;
  }
  return AdventReturn(absl::c_accumulate(counts, int64_t{0}));
}

}  // namespace advent_of_code
