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

absl::StatusOr<Blueprint> Parse(std::string_view line) {
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
  // Resources ordered in the first 32bits with robots in the second for a
  // quick reinterpret_cast cheat on AddResources.
  char ore = 0;
  char clay = 0;
  char obsidian = 0;
  char geode = 0;
  char ore_robot = 1;
  char clay_robot = 0;
  char obsidian_robot = 0;
  char geode_robot = 0;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const State& s) {
    absl::Format(&sink, "O:+%d*%d;C:+%d*%d;D:+%d*%d;G:+%d*%d", s.ore,
                 s.ore_robot, s.clay, s.clay_robot, s.obsidian,
                 s.obsidian_robot, s.geode, s.geode_robot);
  }

  void AddResources() {
    *reinterpret_cast<int32_t*>(&ore) += *reinterpret_cast<int32_t*>(&ore_robot);
  }

  int64_t ForHash() const {
    return *reinterpret_cast<const int64_t*>(this);
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.ForHash());
  }

  bool operator==(const State& o) const {
    return ForHash() == o.ForHash();
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
    int remaining = minutes - i;
    absl::flat_hash_set<State> new_states;
    new_states.reserve(states.size());
    int best_arithmetic_sum = remaining * (remaining - 1) / 2;
    int best_arithmetic_sum_minus_one = (remaining - 2) * (remaining - 1) / 2;
    for (const State& s : states) {
      bool can_build_geode_robot = s.ore >= bp.geode_robot_ore_cost &&
          s.obsidian >= bp.geode_robot_obsidian_cost;

      // The best we could possibly do from this point is to make a new geod
      // robot on every turn all of which are then making geodes. We'll make
      // SUM(geode_robot + i, i=0..remaining) more geodes.
      int best_geode = s.geode + s.geode_robot * remaining;
      if (can_build_geode_robot) {
        best_geode += best_arithmetic_sum;
      } else {
        best_geode += best_arithmetic_sum_minus_one;
      }
      if (best_geode < max_geode) {
        continue;
      }

      max_geode = std::max(max_geode, s.geode + s.geode_robot * remaining);

      if (remaining == 1) continue;

      State next_state = s;
      next_state.AddResources();
      new_states.insert(next_state);

      if (can_build_geode_robot) {
        State build = next_state;
        build.ore -= bp.geode_robot_ore_cost;
        build.obsidian -= bp.geode_robot_obsidian_cost;
        build.geode_robot++;
        new_states.insert(build);
      }

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
    }
    states = std::move(new_states);
  }

  return max_geode;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_19::Part1(
    absl::Span<std::string_view> input) const {
  int cost = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Blueprint bp, Parse(line));
    int best_geode = BestGeode(bp, 24);
    VLOG(1) << "id: " << bp.id << " best is " << best_geode;
    cost += bp.id * best_geode;
  }
  return AdventReturn(cost);
}

absl::StatusOr<std::string> Day_2022_19::Part2(
    absl::Span<std::string_view> input) const {
  int64_t cost = 1;
  input = input.subspan(0, 3);
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Blueprint bp, Parse(line));
    int best_geode = BestGeode(bp, 32);
    VLOG(1) << "id: " << bp.id << " best is " << best_geode;
    cost *= best_geode;
  }
  return AdventReturn(cost);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2022_19()); });

}  // namespace advent_of_code
