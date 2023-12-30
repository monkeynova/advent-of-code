// http://adventofcode.com/2023/day/8

#include "advent_of_code/2023/day08/day08.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"

namespace advent_of_code {

namespace {

class Map {
 public:
  Map() : name_to_id_(26 * 26 * 26, -1) {}

  absl::Status SetDirs(std::string_view lr) {
    dir_.clear();
    dir_.reserve(lr.size());
    for (char c : lr) {
      switch (c) {
        case 'L':
          dir_.push_back(true);
          break;
        case 'R':
          dir_.push_back(false);
          break;
        default:
          return Error("Bad dir");
      }
    }
    return absl::OkStatus();
  }

  int FindLoc(std::array<char, 3> loc) { return name_to_id_[Encode(loc)]; }

  absl::Status AddLine(std::string_view line);

  std::pair<int, int> HistKey(int steps, int loc) const {
    return {steps % dir_.size(), loc};
  }

  const std::vector<int>& GhostStarts() const { return ghost_starts_; }
  const std::vector<bool>& GhostEnds() const { return ghost_ends_; }

  int Advance(int step_idx, int loc) const {
    if (dir_[step_idx]) {
      return map_[loc].first;
    }
    return map_[loc].second;
  }

 private:
  int Encode(std::array<char, 3> loc) {
    return (loc[0] - 'A') * 26 * 26 + (loc[1] - 'A') * 26 + (loc[2] - 'A');
  }

  std::vector<bool> dir_;
  std::vector<int16_t> name_to_id_;
  std::vector<int> ghost_starts_;
  std::vector<bool> ghost_ends_;
  std::vector<std::pair<int16_t, int16_t>> map_;
};

absl::Status Map::AddLine(std::string_view line) {
  if (line.size() != 16) return Error("Bad line");
  int from_loc = Encode({line[0], line[1], line[2]});
  if (name_to_id_[from_loc] == -1) {
    name_to_id_[from_loc] = map_.size();
    map_.push_back({-1, -1});
    ghost_ends_.push_back(false);
  }
  int from_id = name_to_id_[from_loc];
  if (line[2] == 'A')
    ghost_starts_.push_back(from_id);
  else if (line[2] == 'Z')
    ghost_ends_[from_id] = true;

  if (line.substr(3, 4) != " = (") return Error("Bad line");

  int l_loc = Encode({line[7], line[8], line[9]});
  if (name_to_id_[l_loc] == -1) {
    name_to_id_[l_loc] = map_.size();
    map_.push_back({-1, -1});
    ghost_ends_.push_back(false);
  }
  int l_id = name_to_id_[l_loc];

  if (line.substr(10, 2) != ", ") return Error("Bad line");

  int r_loc = Encode({line[12], line[13], line[14]});
  if (name_to_id_[r_loc] == -1) {
    name_to_id_[r_loc] = map_.size();
    map_.push_back({-1, -1});
    ghost_ends_.push_back(false);
  }
  int r_id = name_to_id_[r_loc];

  if (line.substr(15, 1) != ")") return Error("Bad line");

  map_[from_id].first = l_id;
  map_[from_id].second = r_id;

  CHECK_EQ(ghost_ends_.size(), map_.size());

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

  std::optional<int> loc = map.FindLoc({'A', 'A', 'A'});
  if (!loc) return Error("No start");
  std::optional<int> end = map.FindLoc({'Z', 'Z', 'Z'});
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

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/8,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_08()); });

}  // namespace advent_of_code
