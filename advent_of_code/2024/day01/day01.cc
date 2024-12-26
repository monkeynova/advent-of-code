// http://adventofcode.com/2024/day/1

#include "advent_of_code/2024/day01/day01.h"

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
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
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

absl::StatusOr<std::string> Day_2024_01::Part1(
    absl::Span<std::string_view> input) const {
  std::vector<int> l1, l2;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int i1, t.NextInt());
    ASSIGN_OR_RETURN(int i2, t.NextInt());
    if (!t.Done()) {
      return absl::InvalidArgumentError(absl::StrCat(
        "Bad line: ", line));
    }
    l1.push_back(i1);
    l2.push_back(i2);
  }
  absl::c_sort(l1);
  absl::c_sort(l2);
  int sum = 0;
  for (int i = 0; i < l1.size(); ++i) {
    sum += std::abs(l1[i] - l2[i]);
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_01::Part2(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<int64_t, int64_t> l1;
  absl::flat_hash_map<int64_t, int64_t> l2;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int i1, t.NextInt());
    ASSIGN_OR_RETURN(int i2, t.NextInt());
    if (!t.Done()) {
      return absl::InvalidArgumentError(absl::StrCat(
        "Bad line: ", line));
    }
    ++l1[i1];
    ++l2[i2];
  }
  int64_t sum = 0;
  for (const auto& [i, c] : l1) {
    sum += i * c * l2[i];
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_01()); });

}  // namespace advent_of_code
