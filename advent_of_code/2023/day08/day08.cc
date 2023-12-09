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
  Map() = default;

  absl::Status SetDirs(std::string_view lr) {
    dir_.clear();
    dir_.reserve(lr.size());
    for (char c : lr) {
      switch (c) {
        case 'L': dir_.push_back(true); break;
        case 'R': dir_.push_back(false); break;
        default: return Error("Bad dir");
      }
    }
    return absl::OkStatus();
  }

  std::optional<int> FindLoc(std::array<char, 3> loc) {
    auto it = name_to_id_.find(loc);
    if (it == name_to_id_.end()) return std::nullopt;
    return it->second;
  }

  absl::Status AddLine(std::string_view line);

  std::pair<int, int> HistKey(int steps, int loc) const {
    return {steps % dir_.size(), loc};
  }

  std::vector<int> GhostStarts() const;
  std::vector<bool> GhostEnds() const;

  int Advance(int step_idx, int loc) const {
    if (dir_[step_idx]) {
      return map_[loc].first;
    }
    return map_[loc].second;
  }

 private:
  std::vector<bool> dir_;
  absl::flat_hash_map<std::array<char, 3>, int16_t> name_to_id_;
  std::vector<std::pair<int16_t, int16_t>> map_;
};

std::vector<int> Map::GhostStarts() const {
  std::vector<int> ret;
  for (const auto& [name, id] : name_to_id_) {
    if (name.back() == 'A') ret.push_back(id);
  }
  return ret;
}

std::vector<bool> Map::GhostEnds() const {
  std::vector<bool> ret(map_.size(), false);
  for (const auto& [name, id] : name_to_id_) {
    if (name.back() == 'Z') ret[id] = true;
  }
  return ret;
}

absl::Status Map::AddLine(std::string_view line) {
  if (line.size() != 16) return Error("Bad line");
  std::array<char, 3> from = {line[0], line[1], line[2]};
  auto [from_it, from_inserted] = name_to_id_.emplace(from, name_to_id_.size());
  if (from_inserted) {
    map_.push_back({-1, -1});
  }
  int from_id = from_it->second;

  if (line.substr(3, 4) != " = (") return Error("Bad line");

  std::array<char, 3> l = {line[7], line[8], line[9]};
  auto [l_it, l_inserted] = name_to_id_.emplace(l, name_to_id_.size());
  if (l_inserted) {
    map_.push_back({-1, -1});
  }
  int l_id = l_it->second;

  if (line.substr(10, 2) != ", ") return Error("Bad line");

  std::array<char, 3> r = {line[12], line[13], line[14]};
  auto [r_it, r_inserted] = name_to_id_.emplace(r, name_to_id_.size());
  if (r_inserted) {
    map_.push_back({-1, -1});
  }
  int r_id = r_it->second;

  if (line.substr(15, 1) != ")") return Error("Bad line");

  map_[from_id].first = l_id;
  map_[from_id].second = r_id;

  CHECK_EQ(name_to_id_.size(), map_.size());

  return absl::OkStatus();
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


  std::optional<int> loc = map.FindLoc({'A','A','A'});
  if (!loc) return Error("No start");
  std::optional<int> end = map.FindLoc({'Z','Z','Z'});
  if (!end) return Error("No end");

  int dir_size = input[0].size();
  for (int steps = 0, step_idx = 0; true; ++steps) {
    loc = map.Advance(step_idx, *loc);
    if (++step_idx == dir_size) step_idx = 0;
    if (loc == *end) {
      return AdventReturn(steps + 1);
    }
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
  
  int dir_size = input[0].size();
  std::vector<Loop> loops;
  std::vector<bool> ghost_ends = map.GhostEnds();
  int num_locs = ghost_ends.size();
  for (int from : map.GhostStarts()) {
    Loop loop;
    int loc = from;
    std::vector<int> hist(dir_size * num_locs, -1);
    for (int steps = 0, step_idx = 0; true; ++steps) {
      int key = step_idx * num_locs + loc;
      if (hist[key] != -1) {
        loop.from = steps;
        loop.to = hist[key];
        break;
      }
      hist[key] = steps;
      loc = map.Advance(step_idx, loc);
      if (++step_idx == dir_size) step_idx = 0;
      if (ghost_ends[loc]) loop.end_z.push_back(steps + 1);
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
