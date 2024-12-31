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

int64_t Step(int64_t in) {
  in = (in ^ (in << 6)) % 16777216;
  in = (in ^ (in >>5)) % 16777216;
  in = (in ^ (in << 11)) % 16777216;
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

  absl::flat_hash_map<std::array<int8_t, 4>, int64_t> seq2count;
  for (int64_t secret : list) {
    absl::flat_hash_set<std::array<int8_t, 4>> hist;

    int8_t prev, cur;
    std::array<int8_t, 4> seq;
    prev = secret % 10;
    for (int i = 0; i < 4; ++i) {
      secret = Step(secret);
      cur = secret % 10;
      seq[i] = cur - prev;
      prev = cur;
    }
    for (int i = 5; i < 2000; ++i) {
      auto [it, inserted] = hist.insert(seq);
      if (inserted) {
        seq2count[seq] += cur;
      }

      for (int j = 0; j < 3; ++j) {
        seq[j] = seq[j + 1];
      }
      secret = Step(secret);
      cur = secret % 10;
      seq[3] = cur - prev;
      prev = cur;
    }
    secret = Step(secret);
    cur = secret % 10;
    seq[0] = cur - prev;
    prev = cur;
  }
  int64_t max = 0;
  for (const auto& [seq, count] : seq2count) {
    max = std::max(max, count);
  }

  return AdventReturn(max);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/22,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_22()); });

}  // namespace advent_of_code
