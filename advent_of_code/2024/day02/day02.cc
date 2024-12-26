// http://adventofcode.com/2024/day/2

#include "advent_of_code/2024/day02/day02.h"

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

struct Delta {
  Delta(int m1, int m2) {
    if (m1 > m2) { sign = 1; mag = m1 - m2; }
    else if (m1 < m2) { sign = -1; mag = m2 - m1; }
    else { sign = 0; mag = 0; }
  }
  int sign;
  int mag;

  bool Valid() {
    return mag >= 1 && mag <= 3;
  }
};

absl::StatusOr<bool> CheckLine(Delta test, int last, Tokenizer& t) {
  while (!t.Done()) {
    ASSIGN_OR_RETURN(int next, t.NextInt());
    Delta delta(last, next);
    if (!delta.Valid()) {
      return false;
    }
    if (test.sign != delta.sign) {
      return false;
    }
    last = next;
  }
  return true;
}

bool CheckList(std::vector<int> list) {
  for (int i = 1; i + 1 < list.size(); ++i) {
    Delta d1(list[i - 1], list[i]);
    Delta d2(list[i], list[i + 1]);
    if (!d1.Valid()) return false;
    if (!d2.Valid()) return false;
    if (d1.sign != d2.sign) return false;
  }
  return true;
}

bool CheckListWithSkip(std::vector<int> list, int skip) {
  std::vector<int> skipped;
  for (int i = 0; i < list.size(); ++i) {
    if (i == skip) continue;
    skipped.push_back(list[i]);
  }
  return CheckList(skipped);
}

bool CheckListAllowFail(std::vector<int> list) {
  for (int i = 1; i < list.size() - 1; ++i) {
    Delta d1(list[i - 1], list[i]);
    Delta d2(list[i], list[i + 1]);
    if (!d1.Valid()) {
      return CheckListWithSkip(list, i - 1) || CheckListWithSkip(list, i);
    }
    if (!d2.Valid()) {
      return CheckListWithSkip(list, i) || CheckListWithSkip(list, i + 1);      
    }
    if (d1.sign != d2.sign) {
      return CheckListWithSkip(list, i - 1) || CheckListWithSkip(list, i) || CheckListWithSkip(list, i + 1);      

    }
  }
  return true;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_02::Part1(
    absl::Span<std::string_view> input) const {
  int safe_count = 0;
  for (absl::string_view line : input) {
    Tokenizer t(line);
    std::vector<int> list;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int val, t.NextInt());
      list.push_back(val);
    }
    if (CheckList(list)) {
      ++safe_count;
    }
  }
  return AdventReturn(safe_count);
}

absl::StatusOr<std::string> Day_2024_02::Part2(
    absl::Span<std::string_view> input) const {
  int safe_count = 0;
  for (absl::string_view line : input) {
    Tokenizer t(line);
    std::vector<int> list;
    while (!t.Done()) {
      ASSIGN_OR_RETURN(int val, t.NextInt());
      list.push_back(val);
    }
    if (CheckListAllowFail(list)) {
      ++safe_count;
    }
  }
  return AdventReturn(safe_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/2,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_02()); });

}  // namespace advent_of_code
