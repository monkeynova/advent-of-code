// http://adventofcode.com/2024/day/7

#include "advent_of_code/2024/day07/day07.h"

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

bool CanMake(int64_t test, absl::Span<const int64_t> vals) {
  LOG(ERROR) << test << " <-?- " << absl::StrJoin(vals, ",");
  if (vals.size() == 1) return test == vals[0];
  CHECK(!vals.empty());

  if (test < vals.back()) return false;
  if (test % vals.back() == 0 && CanMake(test / vals.back(), vals.subspan(0, vals.size() - 1))) {
    return true;
  }
  return CanMake(test - vals.back(), vals.subspan(0, vals.size() - 1));
}

bool CanMake2(int64_t test, absl::Span<const int64_t> vals) {
  LOG(ERROR) << test << " <-?- " << absl::StrJoin(vals, ",");
  if (vals.size() == 1) return test == vals[0];
  CHECK(!vals.empty());

  if (test < vals.back()) return false;
  absl::Span<const int64_t> tail = vals.subspan(0, vals.size() - 1);

  int64_t pow10 = 1;
  while (pow10 <= vals.back()) pow10 *= 10;
  if (test % pow10 == vals.back() && CanMake2(test / pow10, tail)) {
    return true;
  }
  if (test % vals.back() == 0 && CanMake2(test / vals.back(), tail)) {
    return true;
  }
  return CanMake2(test - vals.back(), tail);
}


}  // namespace

absl::StatusOr<std::string> Day_2024_07::Part1(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int64_t test, t.NextInt());
    RETURN_IF_ERROR(t.NextIs(":"));
    std::vector<int64_t> vals;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int64_t val, t.NextInt());
      if (val < 0) return absl::UnimplementedError("Negative value");
      vals.push_back(val);
    }
    if (CanMake(test, vals)) {
      sum += test;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_07::Part2(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view line : input) {
    Tokenizer t(line);
    ASSIGN_OR_RETURN(int64_t test, t.NextInt());
    RETURN_IF_ERROR(t.NextIs(":"));
    std::vector<int64_t> vals;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int64_t val, t.NextInt());
      if (val < 0) return absl::UnimplementedError("Negative value");
      vals.push_back(val);
    }
    if (CanMake2(test, vals)) {
      sum += test;
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/7,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_07()); });

}  // namespace advent_of_code
