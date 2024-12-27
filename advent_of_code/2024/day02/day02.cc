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

class Delta {
 public:
  Delta(int m1, int m2) {
    if (m1 > m2) { sign_ = 1; mag_ = m1 - m2; }
    else if (m1 < m2) { sign_ = -1; mag_ = m2 - m1; }
    else { sign_ = 0; mag_ = 0; }
  }
 
  bool SameDir(Delta o) const {
    return sign_ == o.sign_;
  }

  bool Valid() const {
    return mag_ >= 1 && mag_ <= 3;
  }

 private:
  int sign_;
  int mag_;
};

bool CheckPartList(Tokenizer t, int last_val, Delta sign) {
  while (!t.Done()) {
    int i = *t.NextInt();
    Delta next(last_val, i);
    if (!next.Valid()) return false;
    if (!sign.SameDir(next)) return false;
    last_val = i;
  }
  return true;
}

bool CheckList(Tokenizer t) {
  int i1 = *t.NextInt();
  if (t.Done()) return true;
  int i2 = *t.NextInt();

  Delta last(i1, i2);
  if (!last.Valid()) return false;
  
  return CheckPartList(t, i2, last);
}

bool CheckListAllowFail(Tokenizer t) {
  int i1 = *t.NextInt();
  if (t.Done()) return true;
  int i2 = *t.NextInt();
  Delta last(i1, i2);
  if (!last.Valid()) {
    int i3 = *t.NextInt();
    {
      Delta d2(i2, i3);
      if (d2.Valid() && CheckPartList(t, i3, d2)) return true;
    }
    {
      Delta d2(i1, i3);
      if (d2.Valid() && CheckPartList(t, i3, d2)) return true;
    }
    return false;
  }
  if (t.Done()) return true;
  int i3 = *t.NextInt();
  Delta next(i2, i3);
  if (!next.Valid()) {
    if (CheckPartList(t, i2, last)) return true;
    next = Delta(i1, i3);
    if (next.Valid() && CheckPartList(t, i3, next)) return true;
    return false;
  }
  if (!last.SameDir(next)) {
    // Drop 2.
    if (CheckPartList(t, i2, last)) return true;
    // Drop 0.
    if (CheckPartList(t, i3, next)) return true;
    next = Delta(i1, i3);
    if (next.Valid() && CheckPartList(t, i3, next)) return true;
    return false;
  }

  last = next;
  while (!t.Done()) {
    int i4 = *t.NextInt();
    Delta next(i3, i4);
    if (!next.Valid() || !last.SameDir(next)) {
      if (CheckPartList(t, i3, last)) return true;
      next = Delta(i2, i4);
      if (next.Valid() && CheckPartList(t, i4, next)) return true;
      return false;
    }
    last = next;
    i2 = i3;
    i3 = i4;
  }
  return true;
}

}  // namespace

absl::StatusOr<std::string> Day_2024_02::Part1(
    absl::Span<std::string_view> input) const {
  int safe_count = 0;
  for (absl::string_view line : input) {
    Tokenizer t(line);
    if (CheckList(t)) {
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
    if (CheckListAllowFail(t)) {
      ++safe_count;
    }
  }
  return AdventReturn(safe_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/2,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_02()); });

}  // namespace advent_of_code
