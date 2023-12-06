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
  struct BySrcStart {
    bool operator()(Range a, Range b) const {
      return a.src_start < b.src_start;
    }
  };

  int64_t Delta() const { return dest_start - src_start; }

  int64_t src_end() const { return src_start + len; }

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
  Interval1D Apply(Interval1D i_int);
  Interval1D ApplyOld(Interval1D i_int) const;

 private:
  std::vector<Range> ranges_;
};

void Map::Apply(std::vector<int64_t>& vals) const {
  for (int64_t& val : vals) {
    for (const Range& r : ranges_) {
      if (val >= r.src_start && val < r.src_start + r.len) {
        val += r.Delta();
        break;
      }
    }
  }
}

Interval1D Map::Apply(Interval1D i_int) {
  Interval1D ret;

  absl::c_sort(ranges_, Range::BySrcStart{});
  absl::Span<const int64_t> i_span = i_int.x();

  auto i_it = i_span.begin();
  auto range_it = ranges_.begin();

  std::optional<int64_t> i_start;
  while (i_it != i_span.end() && range_it != ranges_.end()) {
    if (*i_it < range_it->src_start) {
      if (i_start) {
        ret = ret.Union(Interval1D(*i_start, *i_it));
        i_start = std::nullopt;
      } else {
        i_start = *i_it;
      }

      ++i_it;
      continue;
    }
    if (range_it->src_end() <= *i_it) {
      if (i_start) {
        if (*i_start < range_it->src_start) {
          ret = ret.Union(Interval1D(*i_start, range_it->src_start));
          i_start = range_it->src_start;
        }
        ret = ret.Union(Interval1D(*i_start, range_it->src_end())
                            .Translate(range_it->Delta()));
        i_start = range_it->src_end();        
      }
      ++range_it;
      continue;
    }
    // range_it->src_start <= *i_it < range_it->src_end()
    if (i_start) {
      if (*i_start < range_it->src_start) {
        ret = ret.Union(Interval1D(*i_start, range_it->src_start));
        i_start = range_it->src_start;
      }
      ret = ret.Union(Interval1D(*i_start, *i_it)
                          .Translate(range_it->Delta()));
      i_start = std::nullopt;
    } else {
      i_start = *i_it;
    }
    ++i_it;
  }
  while (i_it != i_span.end()) {
    if (i_start) {
      ret = ret.Union(Interval1D(*i_start, *i_it));
      i_start = std::nullopt;
    } else {
      i_start = *i_it;
    }
    ++i_it;
  }
  CHECK(!i_start);
  return ret;
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