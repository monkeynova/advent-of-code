// http://adventofcode.com/2023/day/12

#include "advent_of_code/2023/day12/day12.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"

namespace advent_of_code {

namespace {

class Memo {
 public:
  Memo(std::string_view max_pre, absl::Span<const int64_t> max_range)
      : pre_(max_pre),
        range_(max_range),
        pre_size_(max_pre.size()),
        range_size_(max_range.size()) {
    map_.resize((pre_size_ + 1) * (range_size_ + 1), -1);
  }

  int64_t CountAllPossible();

 private:
  int64_t CountAllPossible(int pre_off, int range_off);

  int64_t Set(int key, int64_t val) { return map_[key] = val; }

  std::string_view pre_;
  absl::Span<const int64_t> range_;
  std::vector<int> needed_;
  int pre_size_;
  int range_size_;
  std::vector<int64_t> map_;
};

int64_t Memo::CountAllPossible(int pre_off, int range_off) {
  int key = pre_off * (range_size_ + 1) + range_off;
  int64_t from_memo = map_[key];
  if (from_memo != -1) return from_memo;

  if (pre_size_ == pre_off) {
    if (range_.size() == range_off) return map_[key] = 1;
    return map_[key] = 0;
  }
  if (range_size_ == range_off) {
    bool no_pound = true;
    for (int i = pre_off; i < pre_size_; ++i) {
      if (pre_[i] == '#') no_pound = false;
    }
    if (no_pound) {
      return map_[key] = 1;
    }
    return map_[key] = 0;
  }

  if (needed_[range_off] > pre_size_ - pre_off) {
    return map_[key] = 0;
  }

  int64_t total = 0;
  if (pre_[pre_off] == '#' || pre_[pre_off] == '?') {
    int check_size = range_[range_off];
    bool has_space = false;
    for (int i = pre_off + 1; i < pre_off + check_size; ++i) {
      if (pre_[i] == '.') has_space = true;
    }
    if (has_space) {
      // Can't match ('#.#', 3).
    } else if (pre_size_ - pre_off == check_size) {
      if (range_size_ == range_off + 1) {
        ++total;
      }
    } else if (pre_[pre_off + check_size] == '#') {
      // Can't match ('####', 3).
    } else {
      total += CountAllPossible(pre_off + check_size + 1, range_off + 1);
    }
  }
  if (pre_[pre_off] == '.' || pre_[pre_off] == '?') {
    total += CountAllPossible(pre_off + 1, range_off);
  }

  return map_[key] = total;
}

int64_t Memo::CountAllPossible() {
  needed_ = std::vector<int>(range_.size(), 0);
  needed_.back() = range_.back();
  for (int i = range_.size() - 2; i >= 0; --i) {
    needed_[i] = needed_[i + 1] + range_[i] + 1;
  }
  VLOG(2) << "... - " << absl::StrJoin(needed_, ",");
  return CountAllPossible(0, 0);
}

int64_t CountAllPossible(std::string_view pre,
                         absl::Span<const int64_t> range) {
  VLOG(2) << pre << " - " << absl::StrJoin(range, ",");
  return Memo(pre, range).CountAllPossible();
}

}  // namespace

absl::StatusOr<std::string> Day_2023_12::Part1(
    absl::Span<std::string_view> input) const {
  int total = 0;
  for (std::string_view line : input) {
    auto [pre, range_str] = PairSplit(line, " ");
    ASSIGN_OR_RETURN(std::vector<int64_t> range,
                     ParseAsInts(absl::StrSplit(range_str, ",")));
    int all_possible = CountAllPossible(pre, range);
    VLOG(2) << line << " -> " << all_possible;
    total += all_possible;
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_12::Part2(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    auto [pre, range_str] = PairSplit(line, " ");
    std::string pre_duped;
    for (int i = 0; i < 5; ++i) {
      if (i > 0) pre_duped.append("?");
      pre_duped.append(pre);
    }
    ASSIGN_OR_RETURN(std::vector<int64_t> range,
                     ParseAsInts(absl::StrSplit(range_str, ",")));
    std::vector<int64_t> range_duped;
    for (int i = 0; i < 5; ++i) {
      range_duped.insert(range_duped.end(), range.begin(), range.end());
    }
    VLOG(2) << pre_duped << " -> " << absl::StrJoin(range_duped, ",");

    int64_t all_possible = CountAllPossible(pre_duped, range_duped);
    VLOG(2) << line << " -> " << all_possible;
    total += all_possible;
  }
  return AdventReturn(total);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/12,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_12()); });

}  // namespace advent_of_code
