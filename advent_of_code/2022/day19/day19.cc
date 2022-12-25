// http://adventofcode.com/2022/day/19

#include "advent_of_code/2022/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Blueprint {
  int id;
  int ore_robot_ore_cost;
  int clay_robot_ore_cost;
  int obsidian_robot_ore_cost;
  int obsidian_robot_clay_cost;
  int geode_robot_ore_cost;
  int geode_robot_obsidian_cost;
};

absl::StatusOr<Blueprint> Parse(absl::string_view line) {
  Blueprint bp;
  if (!RE2::FullMatch(line,
                      "Blueprint (\\d+): Each ore robot costs (\\d+) ore. "
                      "Each clay robot costs (\\d+) ore. "
                      "Each obsidian robot costs (\\d+) ore and (\\d+) clay. "
                      "Each geode robot costs (\\d+) ore and (\\d+) obsidian.",
                      &bp.id, &bp.ore_robot_ore_cost, &bp.clay_robot_ore_cost,
                      &bp.obsidian_robot_ore_cost, &bp.obsidian_robot_clay_cost,
                      &bp.geode_robot_ore_cost,
                      &bp.geode_robot_obsidian_cost)) {
    return Error("Bad line: ", line);
  }
  return bp;
}

struct State {
  char ore = 0;
  char ore_robot = 1;
  char clay = 0;
  char clay_robot = 0;
  char obsidian = 0;
  char obsidian_robot = 0;
  char geode = 0;
  char geode_robot = 0;

  friend std::ostream& operator<<(std::ostream& o, const State& s) {
    return o << "O:+" << (int)s.ore << "*" << (int)s.ore_robot << ";"
             << "O:+" << (int)s.clay << "*" << (int)s.clay_robot << ";"
             << "O:+" << (int)s.obsidian << "*" << (int)s.obsidian_robot << ";"
             << "O:+" << (int)s.geode << "*" << (int)s.geode_robot << ";";
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.ore, s.ore_robot, s.clay, s.clay_robot,
                      s.obsidian, s.obsidian_robot, s.geode, s.geode_robot);
  }

  bool operator==(const State& o) const {
    return ore == o.ore && ore_robot == o.ore_robot && clay == o.clay &&
           clay_robot == o.clay_robot && obsidian == o.obsidian &&
           obsidian_robot == o.obsidian_robot && geode == o.geode &&
           geode_robot == o.geode_robot;
  }
};

int BestGeode(Blueprint bp, int minutes) {
  absl::flat_hash_set<State> states = {State{}};

  int max_ore_robots =
      std::max(std::max(bp.clay_robot_ore_cost, bp.obsidian_robot_ore_cost),
               bp.geode_robot_ore_cost);
  int max_clay_robots = bp.obsidian_robot_clay_cost;
  int max_obsidian_robots = bp.geode_robot_obsidian_cost;

  int max_geode = 0;
  for (int i = 0; i < minutes; ++i) {
    VLOG(2) << i << ": " << states.size();
    absl::flat_hash_set<State> new_states;
    for (const State& s : states) {
      if (true) {
        int best_geode_add = s.geode_robot * (minutes - i) +
                             (minutes - i) * (minutes - i + 1) / 2;
        if (s.geode + best_geode_add < max_geode) {
          continue;
        }
      }

      max_geode = std::max(max_geode, s.geode + s.geode_robot * (minutes - i));

      State next_state = s;
      next_state.ore += next_state.ore_robot;
      next_state.clay += next_state.clay_robot;
      next_state.obsidian += next_state.obsidian_robot;
      next_state.geode += next_state.geode_robot;
      if (i != minutes - 1) {
        new_states.insert(next_state);
      }
      if (i == minutes - 1) continue;

      if (s.ore >= bp.ore_robot_ore_cost && s.ore_robot < max_ore_robots) {
        State build = next_state;
        build.ore -= bp.ore_robot_ore_cost;
        build.ore_robot++;
        new_states.insert(build);
      }
      if (s.ore >= bp.clay_robot_ore_cost && s.clay_robot < max_clay_robots) {
        State build = next_state;
        build.ore -= bp.clay_robot_ore_cost;
        build.clay_robot++;
        new_states.insert(build);
      }
      if (s.ore >= bp.obsidian_robot_ore_cost &&
          s.clay >= bp.obsidian_robot_clay_cost &&
          s.obsidian_robot < max_obsidian_robots) {
        State build = next_state;
        build.ore -= bp.obsidian_robot_ore_cost;
        build.clay -= bp.obsidian_robot_clay_cost;
        build.obsidian_robot++;
        new_states.insert(build);
      }
      if (s.ore >= bp.geode_robot_ore_cost &&
          s.obsidian >= bp.geode_robot_obsidian_cost) {
        State build = next_state;
        build.ore -= bp.geode_robot_ore_cost;
        build.obsidian -= bp.geode_robot_obsidian_cost;
        build.geode_robot++;
        new_states.insert(build);
      }
    }
    states = std::move(new_states);
  }

  return max_geode;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_19::Part1(
    absl::Span<absl::string_view> input) const {
  int cost = 0;
  for (absl::string_view line : input) {
    absl::StatusOr<Blueprint> bp = Parse(line);
    if (!bp.ok()) return bp.status();
    int best_geode = BestGeode(*bp, 24);
    VLOG(1) << "id: " << bp->id << " best is " << best_geode;
    cost += bp->id * best_geode;
  }
  return AdventReturn(cost);
}

absl::StatusOr<std::string> Day_2022_19::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t cost = 1;
  input = input.subspan(0, 3);
  for (absl::string_view line : input) {
    absl::StatusOr<Blueprint> bp = Parse(line);
    if (!bp.ok()) return bp.status();
    int best_geode = BestGeode(*bp, 32);
    VLOG(1) << "id: " << bp->id << " best is " << best_geode;
    cost *= best_geode;
  }
  return AdventReturn(cost);
}

}  // namespace advent_of_code
