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

struct Map {
  std::string_view lr;
  absl::flat_hash_map<std::string_view, std::pair<std::string_view, std::string_view>> map;

  explicit Map(std::string_view lr_in) : lr(lr_in) {}
  absl::Status AddLine(std::string_view line);
  absl::StatusOr<std::string_view> Advance(int num_step, std::string_view loc) const;
};

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

absl::StatusOr<std::string_view> Map::Advance(
    int num_step, std::string_view loc) const {
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
};

}  // namespace

absl::StatusOr<std::string> Day_2023_08::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");

  Map map(input[0]);
  for (int i = 2; i < input.size(); ++i) {
    RETURN_IF_ERROR(map.AddLine(input[i]));
  }
  
  std::string_view loc = "AAA";
  for (int steps = 0; true; ++steps) {
    ASSIGN_OR_RETURN(loc, map.Advance(steps, loc));
    if (loc == "ZZZ") return AdventReturn(steps + 1);
  }
  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2023_08::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad size");
  if (!input[1].empty()) return Error("Bad input");

  Map map(input[0]);
  for (int i = 2; i < input.size(); ++i) {
    RETURN_IF_ERROR(map.AddLine(input[i]));
  }
  
  std::vector<Loop> loops;
  for (const auto& [from, _] : map.map) {
    if (from.back() == 'A') {
      Loop loop;
      std::string_view loc = from;
      absl::flat_hash_map<std::pair<int, std::string_view>, int> hist;
      for (int steps = 0; true; ++steps) {
        auto [it, inserted] = hist.emplace(std::make_pair(steps % map.lr.size(), loc), steps);
        if (!inserted) {
          loop.from = steps;
          loop.to = it->second;
          break;
        }
        ASSIGN_OR_RETURN(loc, map.Advance(steps, loc));
        if (loc.back() == 'Z') loop.end_z.push_back(steps + 1);
      }
      if (loop.end_z.empty()) return Error("Impossible");
      loops.push_back(loop);
      VLOG(1) << loop.to << "-" << loop.from << ": " << absl::StrJoin(loop.end_z, ",");
    }
  }

  std::optional<int64_t> zero_mod_ok = 1;
  for (const auto& loop : loops) {
    if (loop.end_z.size() != 1) { zero_mod_ok = std::nullopt; break; }
    if (loop.end_z[0] % loop.mod() != 0) { zero_mod_ok = std::nullopt; break; }
    zero_mod_ok = *zero_mod_ok * loop.mod() / std::gcd(*zero_mod_ok, loop.mod());
  }
  if (zero_mod_ok) return AdventReturn(zero_mod_ok);

  std::optional<Loop> probe;
  for (const Loop& loop : loops) {
    if (loop.end_z.size() == 1) {
      probe = loop;
      break;
    }
  }
  if (!probe) return Error("No probe");
  for (int step_test = probe->end_z[0]; true; step_test += probe->mod()) {
    bool all_match = true;
    for (const Loop& loop : loops) {
      bool any_match = false;
      for (int64_t z : loop.end_z) {
        if (step_test < z) continue;
        if (z % loop.mod() == step_test % (loop.mod())) {
          any_match = true;
        }
      }
      if (!any_match) all_match = false;
    }
    if (all_match) {
      return AdventReturn(step_test);
    }
  }

  return Error("Left infinite loop");
}

}  // namespace advent_of_code
