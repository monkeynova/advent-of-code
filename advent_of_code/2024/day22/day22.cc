// http://adventofcode.com/2024/day/22

#include "advent_of_code/2024/day22/day22.h"

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

inline int64_t Step(int64_t in) {
  in = (in ^ (in << 6)) & 0xffffff;
  in = (in ^ (in >> 5)) & 0xffffff;
  in = (in ^ (in << 11)) & 0xffffff;
  return in;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_22::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));
  int64_t sum = 0;
  for (int64_t secret : list) {
    for (int i = 0; i < 2000; ++i) {
      secret = Step(secret);
    }
    sum += secret;
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_22::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> list, ParseAsInts(input));

  // Sequences of 4 values in {-9 .. 9} are encoded as a 4-digit base-19 value.
  const int k19to4 = 130321;

  std::vector<int64_t> seq2count(k19to4, 0);
  for (int64_t secret : list) {
    std::vector<bool> hist(k19to4, false);

    int8_t prev, cur;
    int seq = 0;
    prev = secret % 10;
    for (int i = 0; i < 4; ++i) {
      secret = Step(secret);
      cur = secret % 10;
      seq = ((seq * 19) + 9 + cur - prev) % k19to4;
      prev = cur;
    }
    for (int i = 5; i < 2000; ++i) {
      if (!hist[seq]) {
        hist[seq] = true;
        seq2count[seq] += cur;
      }

      secret = Step(secret);
      cur = secret % 10;
      seq = ((seq * 19) + 9 + cur - prev) % k19to4;
      prev = cur;
    }
  }

  return AdventReturn(absl::c_accumulate(
    seq2count, int64_t{0}, [](int64_t a, int64_t v) {
      return std::max(a, v);
    }));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/22,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_22()); });

}  // namespace advent_of_code
