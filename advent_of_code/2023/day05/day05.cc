// http://adventofcode.com/2023/day/5

#include "advent_of_code/2023/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/interval.h"

namespace advent_of_code {

namespace {

struct Range {
  static absl::StatusOr<Range> Parse(std::string_view line) {
    ASSIGN_OR_RETURN(std::vector<int64_t> vals, ParseAsInts(absl::StrSplit(line, " ")));
    if (vals.size() != 3) return Error("Bad range size");
    return Range{vals[0], vals[1], vals[2]};
  }

  int64_t dest_start;
  int64_t src_start;
  int64_t len;
};

class Map {
 public:
  Map() = default;
  void Clear() { ranges_.clear(); }
  void AddRange(Range range) { ranges_.push_back(range); }

  void Apply(std::vector<int64_t>& vals) const;
  Interval1D Apply(Interval1D i_int) const;

 private:
  std::vector<Range> ranges_;
};

void Map::Apply(std::vector<int64_t>& vals) const {
  for (int64_t& val : vals) {
    for (const Range& r : ranges_) {
      if (val >= r.src_start && val < r.src_start + r.len) {
        val += r.dest_start - r.src_start;
        break;
      }
    }
  }
}

Interval1D Map::Apply(Interval1D i_int) const {
  Interval1D ret;

  // TODO(@monkeynova): Sort ranges, stream intersects.
  for (const Range& r : ranges_) {
    Interval1D overlap =
        i_int.Intersect(Interval1D(r.src_start, r.src_start + r.len));
    if (!overlap.empty()) {
      ret = ret.Union(overlap.Translate(r.dest_start - r.src_start));
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
  type = absl::StripSuffix(type, "s"); // seeds -> seed.

  ASSIGN_OR_RETURN(std::vector<int64_t> vals, ParseAsInts(absl::StrSplit(val_str, " ")));
  if (!input[1].empty()) return Error("No empty line");

  bool was_empty = true;
  Map map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view line = input[i];
    if (was_empty) {
      auto [from, to] = PairSplit(line, "-to-");
      if (type != from) return Error("Bad stream: ", type, ": ", line);
      to = absl::StripSuffix(to, " map:");
      type = to;

      was_empty = false;
    } else if (line.empty()) {
      was_empty = true;
      map.Apply(vals);
      map.Clear();
    } else {
      ASSIGN_OR_RETURN(Range range, Range::Parse(line));
      map.AddRange(range);
    }
  }
  map.Apply(vals);

  return AdventReturn(absl::c_accumulate(
    vals, vals[0],
    [](int64_t a, int64_t b) {
      return std::min(a, b);
    }));
}

absl::StatusOr<std::string> Day_2023_05::Part2(
    absl::Span<std::string_view> input) const {
  auto [type, val_str] = PairSplit(input[0], ": ");
  type = absl::StripSuffix(type, "s"); // seeds -> seed.

  ASSIGN_OR_RETURN(std::vector<int64_t> pre_vals, ParseAsInts(absl::StrSplit(val_str, " ")));
  if (pre_vals.size() % 2 != 0) return Error("Not even");
  Interval1D vals;
  for (int i = 0; i < pre_vals.size(); i += 2) {
    vals = vals.Union(Interval1D(pre_vals[i], pre_vals[i] + pre_vals[i+1]));
  }

  if (!input[1].empty()) return Error("No empty line");
  bool was_empty = true;
  Map map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view line = input[i];
    if (was_empty) {
      auto [from, to] = PairSplit(line, "-to-");
      if (type != from) return Error("Bad stream: ", type, ": ", line);
      to = absl::StripSuffix(to, " map:");
      type = to;

      was_empty = false;
    } else if (line.empty()) {
      was_empty = true;
      vals = map.Apply(vals);
      map.Clear();
    } else {
      ASSIGN_OR_RETURN(Range range, Range::Parse(line));
      map.AddRange(range);
    }
  }
  vals = map.Apply(vals);

  return AdventReturn(vals.x()[0]);
}

}  // namespace advent_of_code