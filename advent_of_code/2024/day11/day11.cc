// http://adventofcode.com/2024/day/11

#include "advent_of_code/2024/day11/day11.h"

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

std::vector<int64_t> NextStones(int64_t stone) {
  if (stone == 0) return {1};

  int64_t pow10_half = 1;
  int base = 0;
  for (int64_t pow10 = 1;; pow10 *= 10) {
    if (pow10 > stone) {
      if (base % 2 == 0) {
       return {stone / pow10_half, stone % pow10_half};
      }
      break;
    }
    if (base % 2 == 1) {
      pow10_half *= 10;
    }
    ++base;
  }

  return {stone * 2024};
}

absl::flat_hash_map<int64_t, int64_t> RunStep(
    const absl::flat_hash_map<int64_t, int64_t>& stones) {
  absl::flat_hash_map<int64_t, int64_t> new_stones;
  for (const auto& [s, c] : stones) {
    for (int64_t new_s : NextStones(s)) {
      new_stones[new_s] += c;
    }
  }
  return new_stones;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_11::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  ASSIGN_OR_RETURN(std::vector<int64_t> in_stones,
                   ParseAsInts(absl::StrSplit(input[0], " ")));
  absl::flat_hash_map<int64_t, int64_t> stones;
  for (int64_t s : in_stones) ++stones[s];

  for (int i = 0; i < 25; ++i) {
    stones = RunStep(stones);
  }
  int64_t total_count = absl::c_accumulate(
    stones, int64_t{0},
    [](int64_t a, const std::pair<int64_t, int64_t>& s_and_c) {
      return a + s_and_c.second;
    });

  return AdventReturn(total_count);
}

absl::StatusOr<std::string> Day_2024_11::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  ASSIGN_OR_RETURN(std::vector<int64_t> in_stones,
                   ParseAsInts(absl::StrSplit(input[0], " ")));
  absl::flat_hash_map<int64_t, int64_t> stones;
  for (int64_t s : in_stones) ++stones[s];

  for (int i = 0; i < 75; ++i) {
    stones = RunStep(stones);
  }
  int64_t total_count = absl::c_accumulate(
    stones, int64_t{0},
    [](int64_t a, const std::pair<int64_t, int64_t>& s_and_c) {
      return a + s_and_c.second;
    });

  return AdventReturn(total_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/11,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_11()); });

}  // namespace advent_of_code
