#include "advent_of_code/2016/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// TODO(@monkeunova): Could use a bit vector rather than sets.

struct Floor {
  std::set<absl::string_view> generators;
  std::set<absl::string_view> microchips;
  bool operator==(const Floor& o) const {
    return generators == o.generators && microchips == o.microchips;
  }
};

template <typename H>
H AbslHashValue(H h, const Floor& f) {
  return H::combine(std::move(h), f.generators, f.microchips);
}

struct State {
  int cur_floor = 0;
  int steps = 0;
  std::vector<Floor> floors;
  bool operator==(const State& o) const {
    return cur_floor == o.cur_floor && floors == o.floors;
  }
};

template <typename H>
H AbslHashValue(H h, const State& s) {
  return H::combine(std::move(h), s.cur_floor, s.floors);
}

std::ostream& operator<<(std::ostream& out, const State& s) {
  for (int i = 0; i < s.floors.size(); ++i) {
    out << "F" << i + 1 << (s.cur_floor == i ? " E": "  ") << ":";
    out << " G{" << absl::StrJoin(s.floors[i].generators, ",") << "}";
    out << " M{" << absl::StrJoin(s.floors[i].microchips, ",") << "}" << std::endl;
  }
  return out;
}

std::vector<State> NextStates(const State& state) {
  int cur_floor_num = state.cur_floor;
  std::vector<int> next_floors;
  if (cur_floor_num - 1 >= 0) next_floors.push_back(cur_floor_num - 1);
  if (cur_floor_num + 1 < state.floors.size()) next_floors.push_back(cur_floor_num + 1);
  const Floor& cur_floor = state.floors[cur_floor_num];

  std::vector<State> ret;
  if (!cur_floor.generators.empty()) {
    for (absl::string_view g : cur_floor.generators) {
      for (int next_floor : next_floors) {
        State next = state;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].generators.insert(g);
        next.floors[cur_floor_num].generators.erase(g);
        ret.push_back(next);
      }
    }
  }
  if (cur_floor.generators.size() > 1) {
    for (absl::string_view g1 : cur_floor.generators) {
      for (absl::string_view g2 : cur_floor.generators) {
        if (g1 >= g2) continue;
        for (int next_floor : next_floors) {
          State next = state;
          ++next.steps;
          next.cur_floor = next_floor;
          next.floors[next_floor].generators.insert(g1);
          next.floors[cur_floor_num].generators.erase(g1);
          next.floors[next_floor].generators.insert(g2);
          next.floors[cur_floor_num].generators.erase(g2);
          ret.push_back(next);
        }
      }
    }
  }
  if (!cur_floor.microchips.empty()) {
    for (absl::string_view g : cur_floor.microchips) {
      for (int next_floor : next_floors) {
        State next = state;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].microchips.insert(g);
        next.floors[cur_floor_num].microchips.erase(g);
        ret.push_back(next);
      }
    }
  }
  if (cur_floor.microchips.size() > 1) {
    for (absl::string_view m1 : cur_floor.microchips) {
      for (absl::string_view m2 : cur_floor.microchips) {
        if (m1 >= m2) continue;
        for (int next_floor : next_floors) {
          State next = state;
          ++next.steps;
          next.cur_floor = next_floor;
          next.floors[next_floor].microchips.insert(m1);
          next.floors[cur_floor_num].microchips.erase(m1);
          next.floors[next_floor].microchips.insert(m2);
          next.floors[cur_floor_num].microchips.erase(m2);
          ret.push_back(next);
        }
      }
    }
  }
  if (!cur_floor.microchips.empty() && !cur_floor.generators.empty()) {
    for (absl::string_view m : cur_floor.microchips) {
      for (absl::string_view g : cur_floor.generators) {
        for (int next_floor : next_floors) {
          State next = state;
          ++next.steps;
          next.cur_floor = next_floor;
          next.floors[next_floor].microchips.insert(m);
          next.floors[cur_floor_num].microchips.erase(m);
          next.floors[next_floor].generators.insert(g);
          next.floors[cur_floor_num].generators.erase(g);
          ret.push_back(next);
        }
      }
    }
  }
  return ret;
}

bool ValidState(const State& state) {
  for (const Floor& f : state.floors) {
    if (!f.generators.empty()) {
      for (absl::string_view microchip : f.microchips) {
        if (f.generators.find(microchip) == f.generators.end()) return false;
      }
    }
  }
  return true;
}

bool FinalState(const State& state) {
  for (int idx = 0; idx < state.floors.size() - 1; ++idx) {
    if (!state.floors[idx].microchips.empty()) return false;
    if (!state.floors[idx].generators.empty()) return false;
  }
  return true;
}

absl::StatusOr<int> StepsToAllOnFourthFloor(State start) {
  std::deque<State> frontier = {start};
  int last_steps = -1;
  absl::flat_hash_set<State> hist;
  while (!frontier.empty()) {
    State s = frontier.front();
    if (s.steps != last_steps) {
      VLOG(1) << "Cur state: (" << s.steps << ")\n" << s;
      last_steps = s.steps;
    }
    VLOG(2) << "Cur state:\n" << s;
    frontier.pop_front();
    std::vector<State> states = NextStates(s);
    for (const State& next : states) {
      VLOG(3) << "Next state:\n" << next;
      if (!hist.contains(next) && ValidState(next)) {
        VLOG(3) << "  Valid!";
        hist.insert(next);
        if (FinalState(next)) return next.steps;
        frontier.push_back(next);
      }
    }
  }
  return AdventDay::Error("No path found");
}

absl::StatusOr<State> ParseInitialState(absl::Span<absl::string_view> input) {
  if (input.size() != 4) return AdventDay::Error("Bad input size");
  State s;
  s.floors.resize(4);
  int floor = 0;
  absl::string_view kFloorNames[] = {"first", "second", "third", "fourth"};
  for (absl::string_view in : input) {
    std::string prefix = absl::StrCat("The ", kFloorNames[floor], " floor contains ");
    if (in.substr(0, prefix.size()) != prefix) {
      return AdventDay::Error("Bad prefix: ", in, " !~ ", prefix);
    }
    in = in.substr(prefix.size());
    if (in == "nothing relevant.") continue;
    std::vector<absl::string_view> components = absl::StrSplit(in, ", ");
    if (components.back().substr(0, 4) == "and ") {
      components.back() = components.back().substr(4);
    }
    if (components.back().back() == '.') {
      components.back() = components.back().substr(0, components.back().size() - 1);
    }
    for (absl::string_view comp : components) {
      absl::string_view e;
      if (RE2::FullMatch(comp, "a (.*) generator", &e)) {
        s.floors[floor].generators.insert(e);
      } else if (RE2::FullMatch(comp, "a (.*)-compatible microchip", &e)) {
        s.floors[floor].microchips.insert(e);
      } else {
        return AdventDay::Error("Bad component: ", comp);
      }
    }
    ++floor;
  }
  return s;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day11_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<State> s = ParseInitialState(input);
  if (!s.ok()) return s.status();
  return IntReturn(StepsToAllOnFourthFloor(*s));
}

absl::StatusOr<std::vector<std::string>> Day11_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<State> s = ParseInitialState(input);
  s->floors[0].generators.insert("elerium");
  s->floors[0].microchips.insert("elerium");
  s->floors[0].generators.insert("dilithium");
  s->floors[0].microchips.insert("dilithium");
  return IntReturn(StepsToAllOnFourthFloor(*s));
}

}  // namespace advent_of_code
