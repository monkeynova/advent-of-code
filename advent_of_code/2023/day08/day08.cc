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

class Map {
 public:
  using LocType = std::string_view;
  static LocType MakeLoc(std::string_view loc) {
    return loc;
  }

  Map() = default;

  absl::Status SetDirs(std::string_view lr_in) {
    lr = lr_in;
    return absl::OkStatus();
  }
  absl::Status AddLine(std::string_view line);

  std::pair<int, LocType> HistKey(int steps, LocType loc) const {
    return {steps % lr.size(), loc};
  }

  std::vector<LocType> GhostStarts() const;
  absl::flat_hash_set<LocType> GhostEnds() const;

  absl::StatusOr<LocType> Advance(int num_step, LocType loc) const;

 private:
  std::string_view lr;
  absl::flat_hash_map<LocType, std::pair<LocType, LocType>> map;
};

std::vector<Map::LocType> Map::GhostStarts() const {
  std::vector<LocType> ret;
  for (const auto& [from, _] : map) {
    if (from.back() == 'A') ret.push_back(from);
  }
  return ret;
}

absl::flat_hash_set<Map::LocType> Map::GhostEnds() const {
  absl::flat_hash_set<LocType> ret;
  for (const auto& [from, _] : map) {
    if (from.back() == 'Z') ret.insert(from);
  }
  return ret;
}

absl::Status Map::AddLine(std::string_view line) {
  if (line.size() != 16) return Error("Bad line");
  std::string_view from = line.substr(0, 3);
  if (line.substr(3, 4) != " = (") return Error("Bad line");
  std::string_view l = line.substr(7, 3);
  if (line.substr(10, 2) != ", ") return Error("Bad line");
  std::string_view r = line.substr(12, 3);
  if (line.substr(15, 1) != ")") return Error("Bad line");
  if (!map.emplace(from, std::make_pair(l, r)).second) {
    return Error("Dup from: ", from);
  }
  return absl::OkStatus();
}

absl::StatusOr<Map::LocType> Map::Advance(int num_step, LocType loc) const {
  char dir = lr[num_step % lr.size()];
  auto it = map.find(loc);
  if (it == map.end()) return Error("Bad loc: ", loc);
  switch (dir) {
    case 'L': return it->second.first;
    case 'R': return it->second.second;
  }
  return Error("Bad dir: ", std::string_view(&dir, 1));
}

struct Loop {
  std::vector<int64_t> end_z;
  int64_t from;
  int64_t to;

  int64_t mod() const { return from - to; }
  bool ZeroAt(int64_t steps) const {
    return absl::c_any_of(
      end_z, [&](int64_t z) { return z % mod() == steps % mod(); });
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Loop& loop) {
    absl::Format(&sink, "%d-%d: %s", loop.to, loop.from,
                 absl::StrJoin(loop.end_z, ","));
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2023_08::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");

  Map map;
  RETURN_IF_ERROR(map.SetDirs(input[0]));
  for (int i = 2; i < input.size(); ++i) {
    RETURN_IF_ERROR(map.AddLine(input[i]));
  }
  
  Map::LocType loc = Map::MakeLoc("AAA");
  Map::LocType end = Map::MakeLoc("ZZZ");
  for (int steps = 0; true; ++steps) {
    ASSIGN_OR_RETURN(loc, map.Advance(steps, loc));
    if (loc == end) return AdventReturn(steps + 1);
  }
  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2023_08::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");

  Map map;
  RETURN_IF_ERROR(map.SetDirs(input[0]));
  for (int i = 2; i < input.size(); ++i) {
    RETURN_IF_ERROR(map.AddLine(input[i]));
  }
  
  std::vector<Loop> loops;
  absl::flat_hash_set<Map::LocType> ghost_ends(map.GhostEnds());
  for (Map::LocType from : map.GhostStarts()) {
    Loop loop;
    Map::LocType loc = from;
    absl::flat_hash_map<std::pair<int, Map::LocType>, int> hist;
    for (int steps = 0; true; ++steps) {
      auto [it, inserted] = hist.emplace(map.HistKey(steps, loc), steps);
      if (!inserted) {
        loop.from = steps;
        loop.to = it->second;
        break;
      }
      ASSIGN_OR_RETURN(loc, map.Advance(steps, loc));
      if (ghost_ends.contains(loc)) loop.end_z.push_back(steps + 1);
    }
    if (loop.end_z.empty()) return Error("Impossible");
    loops.push_back(loop);
    VLOG(1) << loop;
  }

  std::optional<int64_t> zero_mod_ok = 1;
  for (const auto& loop : loops) {
    if (loop.end_z.size() != 1 || loop.end_z[0] % loop.mod() != 0) {
      zero_mod_ok = std::nullopt;
      break;
    }

    int64_t gcd = std::gcd(*zero_mod_ok, loop.mod());
    zero_mod_ok = *zero_mod_ok * loop.mod() / gcd;
  }
  if (zero_mod_ok) return AdventReturn(zero_mod_ok);

  std::optional<Loop> probe;
  for (const Loop& loop : loops) {
    if (loop.end_z.size() == 1) {
      if (!probe || probe->mod() < loop.mod()) {
        probe = loop;
      }
    }
  }
  if (!probe) return Error("No probe");
  VLOG(1) << "Probe = " << *probe;
  for (int64_t step_test = probe->end_z[0]; true; step_test += probe->mod()) {
    if (absl::c_all_of(loops,
                       [&](const Loop& l) { return l.ZeroAt(step_test); })) {
      return AdventReturn(step_test);
    }
  }

  return Error("Left infinite loop");
}

}  // namespace advent_of_code
