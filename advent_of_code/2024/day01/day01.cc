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
  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2024_01::Part2(
    absl::Span<std::string_view> input) const {
  return absl::UnimplementedError("Problem not known");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_01()); });

}  // namespace advent_of_code
