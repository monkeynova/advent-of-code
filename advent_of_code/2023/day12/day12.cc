// http://adventofcode.com/2023/day/12

#include "advent_of_code/2023/day12/day12.h"

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

class Memo {
 public:
  Memo(std::string_view max_pre, absl::Span<const int64_t> max_range)
   : max_pre_(max_pre.size() + 1), max_range_(max_range.size() + 1) {
    max_cur_range_ = 2 + absl::c_accumulate(
      max_range, static_cast<int64_t>(0),
      [](int64_t a, int64_t b) { return std::max(a, b); });
    map_.resize(max_pre_ * max_range_ * max_cur_range_, -1);
  }

  int Key(std::string_view pre, absl::Span<const int64_t> range,
          int cur_range) {
    return pre.size() * max_range_ * max_cur_range_ +
      range.size() * max_cur_range_ +
      cur_range;
  }

  int64_t Find(int key) {
    return map_[key];
  }

  int64_t Set(int key, int64_t val) {
    return map_[key] = val;
  }

 private:
  int max_pre_;
  int max_range_;
  int max_cur_range_;
  std::vector<int64_t> map_;
};

int64_t CountAllPossible(
    Memo* memo, std::string_view pre, absl::Span<const int64_t> range,
    absl::Span<const int> needed, int cur_range) {

  int key = memo->Key(pre, range, cur_range);
  int64_t from_memo = memo->Find(key);
  if (from_memo != -1) return from_memo;

  CHECK_EQ(range.size(), needed.size());

  if (pre.empty()) {
    if (cur_range == 0 && range.empty()) return memo->Set(key, 1);
    if (range.size() == 1 && cur_range == range[0]) return memo->Set(key, 1);
    return memo->Set(key, 0);
  }
  if (range.empty() && cur_range > 0) return memo->Set(key, 0);
  if (!range.empty() && cur_range > range[0]) return memo->Set(key, 0);

  if (!needed.empty() && needed[0] > cur_range + pre.size()) {
    return memo->Set(key, 0);
  }

  int64_t total = 0;
  if (pre[0] == '#' || pre[0] == '?') {
    total +=
        CountAllPossible(memo, pre.substr(1), range, needed, cur_range + 1);
  }
  if (pre[0] == '.' || pre[0] == '?') {
    if (range.empty()) {
      if (cur_range == 0) {
        total += CountAllPossible(memo, pre.substr(1), range, needed, 0);
      }
    } else if (cur_range == 0) {
      total += CountAllPossible(memo, pre.substr(1), range, needed, 0);
    } else if (range[0] == cur_range) {
      total += CountAllPossible(memo, pre.substr(1), range.subspan(1),
                                needed.subspan(1), 0);
    }  
  }

  return memo->Set(key, total);
}

int64_t CountAllPossible(std::string_view pre, absl::Span<const int64_t> range) {
  Memo memo(pre, range);
  std::vector<int> needed(range.size(), 0);
  needed[range.size() - 1] = range[range.size() - 1];
  for (int i = range.size() - 2; i >= 0; --i) {
    needed[i] = needed[i + 1] + range[i] + 1;
  }
  VLOG(2) << pre << " - " << absl::StrJoin(range, ",");
  VLOG(2) << "... - " << absl::StrJoin(needed, ",");
  return CountAllPossible(&memo, pre, range, needed, 0);
}

}  // namespace

absl::StatusOr<std::string> Day_2023_12::Part1(
    absl::Span<std::string_view> input) const {
  int total = 0;
  for (std::string_view line : input) {
    auto [pre, range_str] = PairSplit(line, " ");
    ASSIGN_OR_RETURN(std::vector<int64_t> range, ParseAsInts(absl::StrSplit(range_str, ",")));
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
    ASSIGN_OR_RETURN(std::vector<int64_t> range, ParseAsInts(absl::StrSplit(range_str, ",")));
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

}  // namespace advent_of_code
