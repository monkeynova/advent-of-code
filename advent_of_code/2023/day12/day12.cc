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
  class Key {
   public:
    Key(std::string_view pre, absl::Span<const int64_t> range, int cur_range)
     : pre_size_(pre.size()), range_size_(range.size()), cur_range_(cur_range) {}
  
    bool operator==(const Key& o) const {
      return pre_size_ == o.pre_size_ && range_size_ == o.range_size_ && cur_range_ == o.cur_range_;
    }
    template <typename H>
    friend H AbslHashValue(H h, const Key& m) {
      return H::combine(std::move(h), m.pre_size_, m.range_size_, m.cur_range_);
    }
   private:
    friend class Memo;

    int pre_size_;
    int range_size_;
    int cur_range_;  
  };

  Memo(std::string_view max_pre, absl::Span<const int64_t> max_range)
   : max_pre_(max_pre.size() + 1), max_range_(max_range.size() + 1) {
    max_cur_range_ = 1 + absl::c_accumulate(
      max_range, static_cast<int64_t>(0),
      [](int64_t a, int64_t b) { return std::max(a, b); });
    map_.resize(max_pre_ * max_range_ * max_cur_range_, -1);
  }

  int64_t Find(const Key& key) {
    return map_[KeyIdx(key)];
  }

  int64_t Set(const Key& key, int64_t val) {
    return map_[KeyIdx(key)] = val;
  }

 private:
  int KeyIdx(const Key& key) {
    return key.pre_size_ * max_range_ * max_cur_range_ +
      key.range_size_ * max_cur_range_ +
      key.cur_range_;
  }

  int max_pre_;
  int max_range_;
  int max_cur_range_;
  std::vector<int64_t> map_;
};

int64_t CountAllPossible(Memo* memo, std::string_view pre, absl::Span<const int64_t> range, int cur_range) {
  VLOG(3) << "CountAllPossible(" << pre << ",{" << absl::StrJoin(range, ",") << "}, " << cur_range << ")";
  if (pre.empty()) {
    if (cur_range == 0 && range.empty()) return 1;
    if (range.size() == 1 && cur_range == range[0]) return 1;
    return 0;
  }
  if (range.empty() && cur_range > 0) return 0;
  if (!range.empty() && cur_range > range[0]) return 0;

  Memo::Key key(pre, range, cur_range);
  int64_t from_memo = memo->Find(key);
  if (from_memo != -1) return from_memo;

  int64_t total = 0;
  if (pre[0] == '#' || pre[0] == '?') {
    total += CountAllPossible(memo, pre.substr(1), range, cur_range + 1);
  }
  if (pre[0] == '.' || pre[0] == '?') {
    if (range.empty()) {
      if (cur_range == 0) {
        total += CountAllPossible(memo, pre.substr(1), range, 0);
      }
    } else if (cur_range == 0) {
      total += CountAllPossible(memo, pre.substr(1), range, 0);
    } else if (range[0] == cur_range) {
      total += CountAllPossible(memo, pre.substr(1), range.subspan(1), 0);
    }  
  }
  return memo->Set(key, total);
}

int64_t CountAllPossible(std::string_view pre, absl::Span<const int64_t> range) {
  Memo memo(pre, range);
  return CountAllPossible(&memo, pre, range, 0);
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
