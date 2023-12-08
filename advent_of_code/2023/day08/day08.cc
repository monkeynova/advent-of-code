// http://adventofcode.com/2023/day/8

#include "advent_of_code/2023/day08/day08.h"

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
#include "advent_of_code/mod.h"
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

absl::StatusOr<std::string> Day_2023_08::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");
  std::string_view lr = input[0];
  absl::flat_hash_map<std::string_view, std::pair<std::string_view, std::string_view>> map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view from, l, r;
    if (!RE2::FullMatch(input[i],
                        "([A-Z][A-Z][A-Z]) = \\(([A-Z][A-Z][A-Z]), ([A-Z][A-Z][A-Z])\\)",
                        &from, &l, &r)) {
      return Error("Bad line: ", input[i]);
    }
    if (!map.emplace(from, std::make_pair(l, r)).second) {
      return Error("Dup from: ", from);
    }
  }
  
  int steps = 0;
  for (std::string_view loc = "AAA"; loc != "ZZZ"; ++steps) {
    char dir = lr[steps % lr.size()];
    auto it = map.find(loc);
    if (it == map.end()) return Error("Bad loc: ", loc);
    switch (dir) {
      case 'L': loc = it->second.first; break;
      case 'R': loc = it->second.second; break;
      default: return Error("Bad dir: ", std::string_view(&dir, 1));
    }
  }
  return AdventReturn(steps);
}

absl::StatusOr<std::string> Day_2023_08::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");
  std::string_view lr = input[0];
  absl::flat_hash_map<std::string_view, std::pair<std::string_view, std::string_view>> map;
  for (int i = 2; i < input.size(); ++i) {
    std::string_view from, l, r;
    if (!RE2::FullMatch(input[i],
                        "([A-Z1-9][A-Z1-9][A-Z]) = \\(([A-Z1-9][A-Z1-9][A-Z]), ([A-Z1-9][A-Z1-9][A-Z])\\)",
                        &from, &l, &r)) {
      return Error("Bad line: ", input[i]);
    }
    if (!map.emplace(from, std::make_pair(l, r)).second) {
      return Error("Dup from: ", from);
    }
  }
  
  int steps = 0;
  struct Loop {
    std::vector<int64_t> end_z;
    int64_t from;
    int64_t to;
  };
  std::vector<Loop> loops;
  std::vector<std::string_view> manypos;
  for (const auto& [from, _] : map) {
    if (from.back() == 'A') {
      manypos.push_back(from);
      Loop loop;
      std::string_view loc = from;
      absl::flat_hash_map<std::pair<int, std::string_view>, int> hist;
      int steps;
      for (steps = 0; !hist.contains(std::make_pair(steps % lr.size(), loc)); ++steps) {
        // VLOG(1) << loc;
        hist.emplace(std::make_pair(steps % lr.size(), loc), steps);
        char dir = lr[steps % lr.size()];
        auto it = map.find(loc);
        if (it == map.end()) return Error("Bad loc: ", loc);
        switch (dir) {
          case 'L': loc = it->second.first; break;
          case 'R': loc = it->second.second; break;
          default: return Error("Bad dir: ", std::string_view(&dir, 1));
        }
        if (loc.back() == 'Z') loop.end_z.push_back(steps + 1);
      }
      // VLOG(1) << loc;
      if (loop.end_z.empty()) return Error("Impossible");
      loop.from = steps;
      loop.to = hist[std::make_pair(steps % lr.size(), loc)];
      loops.push_back(loop);
      VLOG(1) << loop.to << "-" << loop.from << ": " << absl::StrJoin(loop.end_z, ",");
    }
  }

  std::optional<int64_t> zero_mod_ok = 1;
  for (const auto& loop : loops) {
    if (loop.end_z.size() != 1) { zero_mod_ok = std::nullopt; break; }
    int64_t mod = loop.from - loop.to;
    if (loop.end_z[0] % mod != 0) { zero_mod_ok = std::nullopt; break; }
    zero_mod_ok = *zero_mod_ok * mod / std::gcd(*zero_mod_ok, mod);
  }
  if (zero_mod_ok) return AdventReturn(zero_mod_ok);

#if 0
  std::vector<int> indexes(loops.size(), 0);
  int64_t min = std::numeric_limits<int64_t>::max();
  while (true) {
    VLOG(1) << absl::StrJoin(indexes, ",");
    std::vector<std::pair<int64_t, int64_t>> for_remainder;
    for (int i = 0; i < loops.size(); ++i) {
      const Loop& loop = loops[i];
      int64_t mod = loop.from - loop.to;
      int64_t find = loop.end_z[indexes[i]] % mod;
      for_remainder.push_back({find, mod});
      VLOG(1) << "find " << find << " MOD " << mod;
    }
    std::optional<int64_t> found = ChineseRemainder(for_remainder);
    if (found) {
      min = std::min(min, *found);
    }
    bool done = true;
    for (int index_idx = indexes.size() - 1; index_idx >= 0; --index_idx) {
      if (++indexes[index_idx] < loops[index_idx].end_z.size()) {
        done = false;
        break;
      }
      indexes[index_idx] = 0;
    }
    VLOG(1) << done;
    if (done) break;
  }
  return AdventReturn(min);
#endif

  std::optional<Loop> probe;
  for (const Loop& loop : loops) {
    if (loop.end_z.size() == 1) {
      probe = loop;
      break;
    }
  }
  if (!probe) return Error("No probe");
  for (int step_test = probe->end_z[0]; true; step_test += probe->from - probe->to) {
    bool all_match = true;
    for (const Loop& loop : loops) {
      bool any_match = false;
      for (int64_t z : loop.end_z) {
        if (step_test < z) continue;
        if (z % (loop.from - loop.to) == step_test % (loop.from - loop.to)) {
          any_match = true;
        }
      }
      if (!any_match) all_match = false;
    }
    if (all_match) {
      return AdventReturn(step_test);
    }
  }
  for (bool done = false; !done; ++steps) {
    done = true;
    char dir = lr[steps % lr.size()];
    for (std::string_view& loc : manypos) {
      auto it = map.find(loc);
      if (it == map.end()) return Error("Bad loc: ", loc);
      switch (dir) {
        case 'L': loc = it->second.first; break;
        case 'R': loc = it->second.second; break;
        default: return Error("Bad dir: ", std::string_view(&dir, 1));
      }
      if (loc.back() != 'Z') done = false;
    }
  }
  return AdventReturn(steps);
}

}  // namespace advent_of_code
