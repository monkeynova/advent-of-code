// http://adventofcode.com/2024/day/2

#include "advent_of_code/2024/day02/day02.h"

#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

class Delta {
 public:
  Delta(int m1, int m2) {
    if (m1 > m2) {
      sign_ = 1;
      mag_ = m1 - m2;
    } else if (m1 < m2) {
      sign_ = -1;
      mag_ = m2 - m1;
    } else {
      sign_ = 0;
      mag_ = 0;
    }
  }

  bool SameDir(Delta o) const { return sign_ == o.sign_; }

  bool Valid() const { return mag_ >= 1 && mag_ <= 3; }

 private:
  int sign_;
  int mag_;
};

absl::StatusOr<bool> CheckPartList(Tokenizer t, int last_val, Delta sign) {
  while (!t.Done()) {
    ASSIGN_OR_RETURN(int i, t.NextInt());
    Delta next(last_val, i);
    if (!next.Valid()) return false;
    if (!sign.SameDir(next)) return false;
    last_val = i;
  }
  return true;
}

absl::StatusOr<bool> CheckList(Tokenizer t) {
  ASSIGN_OR_RETURN(int i1, t.NextInt());
  if (t.Done()) return true;
  ASSIGN_OR_RETURN(int i2, t.NextInt());

  Delta last(i1, i2);
  if (!last.Valid()) return false;

  return CheckPartList(t, i2, last);
}

absl::StatusOr<bool> CheckListAllowFail(Tokenizer t) {
  bool sub_part_check;
  ASSIGN_OR_RETURN(int i1, t.NextInt());
  if (t.Done()) return true;
  ASSIGN_OR_RETURN(int i2, t.NextInt());
  Delta last(i1, i2);
  if (!last.Valid()) {
    ASSIGN_OR_RETURN(int i3, t.NextInt());
    {
      // Drop i1.
      Delta d2(i2, i3);
      if (d2.Valid()) {
        ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, d2));
        if (sub_part_check) return true;
      }
    }
    {
      // Drop i2.
      Delta d2(i1, i3);
      if (d2.Valid()) {
        ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, d2));
        if (sub_part_check) return true;
      }
    }
    return false;
  }
  if (t.Done()) return true;
  ASSIGN_OR_RETURN(int i3, t.NextInt());
  Delta next(i2, i3);
  if (!next.Valid()) {
    // Drop i3.
    ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i2, last));
    if (sub_part_check) return true;
    // Drop i2.
    next = Delta(i1, i3);
    if (next.Valid()) {
      ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, next));
      if (sub_part_check) return true;
    }
    return false;
  }
  if (!last.SameDir(next)) {
    // Drop i3.
    ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i2, last));
    if (sub_part_check) return true;
    // Drop i1.
    ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, next));
    if (sub_part_check) return true;
    // Drop i2.
    next = Delta(i1, i3);
    if (next.Valid()) {
      ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, next));
      if (sub_part_check) return true;
    }
    return false;
  }

  last = next;
  while (!t.Done()) {
    ASSIGN_OR_RETURN(int i4, t.NextInt());
    Delta next(i3, i4);
    if (!next.Valid() || !last.SameDir(next)) {
      //  Drop i4.
      ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i3, last));
      if (sub_part_check) return true;
      // Drop i3.
      next = Delta(i2, i4);
      if (next.Valid()) {
        ASSIGN_OR_RETURN(sub_part_check, CheckPartList(t, i4, next));
        if (sub_part_check) return true;
      }
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
    ASSIGN_OR_RETURN(bool is_safe, CheckList(Tokenizer(line)));
    if (is_safe) ++safe_count;
  }
  return AdventReturn(safe_count);
}

absl::StatusOr<std::string> Day_2024_02::Part2(
    absl::Span<std::string_view> input) const {
  int safe_count = 0;
  for (absl::string_view line : input) {
    ASSIGN_OR_RETURN(bool is_safe, CheckListAllowFail(Tokenizer(line)));
    if (is_safe) ++safe_count;
  }
  return AdventReturn(safe_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/2,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_02()); });

}  // namespace advent_of_code
