// http://adventofcode.com/2023/day/6

#include "advent_of_code/2023/day06/day06.h"

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

absl::StatusOr<std::string> Day_2023_06::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  std::vector<int> times;
  {
    Tokenizer tok(input[0]);
    if (tok.Next() != "Time") return Error("Bad time");
    if (tok.Next() != ":") return Error("Bad time");
    while (!tok.Done()) {
      int time;
      if (!absl::SimpleAtoi(tok.Next(), &time)) return Error("Bad time");
      times.push_back(time);
    }
  }
  std::vector<int> distances;
  {
    Tokenizer tok(input[1]);
    if (tok.Next() != "Distance") return Error("Bad distance");
    if (tok.Next() != ":") return Error("Bad distance");
    while (!tok.Done()) {
      int distance;
      if (!absl::SimpleAtoi(tok.Next(), &distance)) return Error("Bad distance");
      distances.push_back(distance);
    }
  }
  if (times.size() != distances.size()) return Error("Bad lengths");
  int64_t product = 1;
  for (int i = 0; i < times.size(); ++i) {
    int valid = 0;
    for (int test = 1; test < times[i]; ++test) {
      if ((times[i] - test) * test > distances[i]) {
        ++valid;
      }
    }
    product *= valid;
  }
  return AdventReturn(product);
}

absl::StatusOr<std::string> Day_2023_06::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int64_t time = 0;
  for (char c: input[0]) {
    if (isdigit(c)) {
      time = time * 10 + c - '0';
    }
  }
  int64_t distance = 0;
  for (char c: input[1]) {
    if (isdigit(c)) {
      distance = distance * 10 + c - '0';
    }
  }
  int64_t valid = 0;
  for (int64_t test = 1; test < time; ++test) {
    if ((time - test) * test > distance) {
      ++valid;
    }
  }
  return AdventReturn(valid);
}

}  // namespace advent_of_code
