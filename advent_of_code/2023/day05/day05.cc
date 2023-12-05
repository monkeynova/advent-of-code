// http://adventofcode.com/2023/day/5

#include "advent_of_code/2023/day05/day05.h"

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

struct Range {
  int64_t dest_start;
  int64_t src_start;
  int64_t len;
};

int64_t ApplyMap(const std::vector<Range>& ranges, int64_t val) {
  for (const Range& r : ranges) {
    if (val >= r.src_start && val < r.src_start + r.len) {
      return val + r.dest_start - r.src_start;
    }
  }
  return val;  
}

struct Segment {
  int64_t start;
  int64_t end;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Segment& seg) {
    absl::Format(&sink, "[%v,%v)", seg.start, seg.end);
  }
};

Interval1D ApplyMap(const std::vector<Range>& ranges, Interval1D i_int) {
  Interval1D ret;

  for (const Range& r : ranges) {
    Interval1D r_int(r.src_start, r.src_start + r.len);
    Interval1D overlap = r_int.Intersect(i_int);
    if (!overlap.empty()) {
      absl::Span<const int64_t> r_range = overlap.x();
      CHECK_EQ(r_range.size() % 2, 0);
      for (int i = 0; i < r_range.size(); i += 2) {
        ret = ret.Union(Interval1D(
          r_range[i] + r.dest_start - r.src_start,
          r_range[i + 1] + r.dest_start - r.src_start
        ));
      }
      i_int = i_int.Minus(overlap);
    }
    if (i_int.empty()) break;
  }
  return ret.Union(i_int);
}


}  // namespace

absl::StatusOr<std::string> Day_2023_05::Part1(
    absl::Span<std::string_view> input) const {
  auto [type, val_str] = PairSplit(input[0], ": ");
  if (type != "seeds") return Error("Bad start");
  type = "seed";
  ASSIGN_OR_RETURN(std::vector<int64_t> vals, ParseAsInts(absl::StrSplit(val_str, " ")));
  if (!input[1].empty()) return Error("No empty line");
  bool was_empty = true;
  std::vector<Range> map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view line = input[i];
    if (was_empty) {
      std::string_view from;
      std::string_view to;
      if (!RE2::FullMatch(line, "(\\w+)\\-to\\-(\\w+) map:", &from, &to)) {
        return Error("Bad map: ", line);
      }
      if (type != from) return Error("Bad stream: ", type, ": ", line);
      type = to;
      was_empty = false;
    } else if (line.empty()) {
      was_empty = true;
      for (int64_t& v : vals) {
        v = ApplyMap(map, v);
      }
      map = {};
    } else {
      ASSIGN_OR_RETURN(std::vector<int64_t> range, ParseAsInts(absl::StrSplit(line, " ")));
      if (range.size() != 3) return Error("Bad range size");
      map.push_back({range[0], range[1], range[2]});
    }
  }
  for (int64_t& v : vals) {
    v = ApplyMap(map, v);
  }

  int64_t min = absl::c_accumulate(vals, std::numeric_limits<int64_t>::max(), [](int64_t a, int64_t b) {
    return std::min(a, b);
  });
  return AdventReturn(min);
}

absl::StatusOr<std::string> Day_2023_05::Part2(
    absl::Span<std::string_view> input) const {
  auto [type, val_str] = PairSplit(input[0], ": ");
  if (type != "seeds") return Error("Bad start");
  type = "seed";
  ASSIGN_OR_RETURN(std::vector<int64_t> pre_vals, ParseAsInts(absl::StrSplit(val_str, " ")));
  if (pre_vals.size() % 2 != 0) return Error("Not even");
  Interval1D vals;
  for (int i = 0; i < pre_vals.size(); i += 2) {
    vals = vals.Union(Interval1D(pre_vals[i], pre_vals[i] + pre_vals[i+1]));
  }
  VLOG(1) << vals;

  if (!input[1].empty()) return Error("No empty line");
  bool was_empty = true;
  std::vector<Range> map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view line = input[i];
    if (was_empty) {
      std::string_view from;
      std::string_view to;
      if (!RE2::FullMatch(line, "(\\w+)\\-to\\-(\\w+) map:", &from, &to)) {
        return Error("Bad map: ", line);
      }
      if (type != from) return Error("Bad stream: ", type, ": ", line);
      type = to;
      was_empty = false;
    } else if (line.empty()) {
      was_empty = true;
      vals = ApplyMap(map, vals);
      map = {};
    } else {
      ASSIGN_OR_RETURN(std::vector<int64_t> range, ParseAsInts(absl::StrSplit(line, " ")));
      if (range.size() != 3) return Error("Bad range size");
      map.push_back({range[0], range[1], range[2]});
    }
  }
  vals = ApplyMap(map, vals);

  absl::Span<const int64_t> x = vals.x();
  if (x.size() % 2 != 0) return Error("Bad parity");
  int64_t min = std::numeric_limits<int64_t>::max();
  for (int i = 0; i < x.size(); i += 2) {
    min = std::min<int64_t>(min, x[i]);
  }
  return AdventReturn(min);
}

}  // namespace advent_of_code