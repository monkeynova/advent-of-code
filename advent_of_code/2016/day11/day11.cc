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

// TODO(@monkeunova): We could probably A* the search with a
//                    SUM(dist_to_4th_floor) / 2 metric.

struct Floor {
  int generators_bv;
  int microchips_bv;
  bool operator==(const Floor& o) const {
    return generators_bv == o.generators_bv && microchips_bv == o.microchips_bv;
  }
};

template <typename H>
H AbslHashValue(H h, const Floor& f) {
  return H::combine(std::move(h), f.generators_bv, f.microchips_bv);
}

struct State {
  int cur_floor = 0;
  int steps = 0;
  std::vector<Floor> floors;
  std::vector<absl::string_view> elements;
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
    out << "F" << i + 1 << (s.cur_floor == i ? " E" : "  ") << ":";
    out << " G{";
    for (int bit_index = 0; (1 << bit_index) <= s.floors[i].generators_bv;
         ++bit_index) {
      if (s.floors[i].generators_bv & (1 << bit_index)) {
        out << s.elements[bit_index] << ",";
      }
    }
    out << "}";
    out << " M{";
    for (int bit_index = 0; (1 << bit_index) <= s.floors[i].microchips_bv;
         ++bit_index) {
      if (s.floors[i].microchips_bv & (1 << bit_index)) {
        out << s.elements[bit_index] << ",";
      }
    }
    out << "}\n";
  }
  return out;
}

std::vector<State> NextStates(const State& state) {
  int cur_floor_num = state.cur_floor;
  std::vector<int> next_floors;
  if (cur_floor_num - 1 >= 0) next_floors.push_back(cur_floor_num - 1);
  if (cur_floor_num + 1 < state.floors.size())
    next_floors.push_back(cur_floor_num + 1);
  const Floor& cur_floor = state.floors[cur_floor_num];

  std::vector<State> ret;
  for (int bit_index = 0; (1 << bit_index) <= cur_floor.generators_bv;
       ++bit_index) {
    if (!(cur_floor.generators_bv & (1 << bit_index))) continue;
    for (int next_floor : next_floors) {
      State next = state;
      ++next.steps;
      next.cur_floor = next_floor;
      next.floors[next_floor].generators_bv |= (1 << bit_index);
      next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index);
      ret.push_back(next);
    }
  }
  for (int bit_index1 = 0; (1 << bit_index1) <= cur_floor.generators_bv;
       ++bit_index1) {
    if (!(cur_floor.generators_bv & (1 << bit_index1))) continue;
    for (int bit_index2 = 0; bit_index2 < bit_index1; ++bit_index2) {
      if (!(cur_floor.generators_bv & (1 << bit_index2))) continue;
      for (int next_floor : next_floors) {
        State next = state;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].generators_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index1);
        next.floors[next_floor].generators_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index2);
        ret.push_back(next);
      }
    }
  }
  for (int bit_index = 0; (1 << bit_index) <= cur_floor.microchips_bv;
       ++bit_index) {
    if (!(cur_floor.microchips_bv & (1 << bit_index))) continue;
    for (int next_floor : next_floors) {
      State next = state;
      ++next.steps;
      next.cur_floor = next_floor;
      next.floors[next_floor].microchips_bv |= (1 << bit_index);
      next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index);
      ret.push_back(next);
    }
  }
  for (int bit_index1 = 0; (1 << bit_index1) <= cur_floor.microchips_bv;
       ++bit_index1) {
    if (!(cur_floor.microchips_bv & (1 << bit_index1))) continue;
    for (int bit_index2 = 0; bit_index2 < bit_index1; ++bit_index2) {
      if (!(cur_floor.microchips_bv & (1 << bit_index2))) continue;
      for (int next_floor : next_floors) {
        State next = state;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].microchips_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index1);
        next.floors[next_floor].microchips_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index2);
        ret.push_back(next);
      }
    }
  }
  for (int bit_index1 = 0; (1 << bit_index1) <= cur_floor.microchips_bv;
       ++bit_index1) {
    if (!(cur_floor.microchips_bv & (1 << bit_index1))) continue;
    for (int bit_index2 = 0; (1 << bit_index2) <= cur_floor.generators_bv;
         ++bit_index2) {
      if (!(cur_floor.generators_bv & (1 << bit_index2))) continue;
      for (int next_floor : next_floors) {
        State next = state;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].microchips_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index1);
        next.floors[next_floor].generators_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index2);
        ret.push_back(next);
      }
    }
  }
  return ret;
}

bool ValidState(const State& state) {
  for (const Floor& f : state.floors) {
    if (f.generators_bv != 0) {
      if (f.microchips_bv & ~f.generators_bv) return false;
    }
  }
  return true;
}

bool FinalState(const State& state) {
  for (int idx = 0; idx < state.floors.size() - 1; ++idx) {
    if (state.floors[idx].microchips_bv != 0) return false;
    if (state.floors[idx].generators_bv != 0) return false;
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
  absl::flat_hash_map<absl::string_view, int> element_to_id;
  for (absl::string_view in : input) {
    std::string prefix =
        absl::StrCat("The ", kFloorNames[floor], " floor contains ");
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
      components.back() =
          components.back().substr(0, components.back().size() - 1);
    }
    for (absl::string_view comp : components) {
      absl::string_view e;
      if (RE2::FullMatch(comp, "a (.*) generator", &e)) {
        if (!element_to_id.contains(e)) {
          element_to_id[e] = s.elements.size();
          s.elements.push_back(e);
        }
        s.floors[floor].generators_bv |= (1 << element_to_id[e]);
      } else if (RE2::FullMatch(comp, "a (.*)-compatible microchip", &e)) {
        if (!element_to_id.contains(e)) {
          element_to_id[e] = s.elements.size();
          s.elements.push_back(e);
        }
        s.floors[floor].microchips_bv |= (1 << element_to_id[e]);
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
  std::string elerium = "elerium";
  std::string dilithium = "dilithium";
  int e_index = s->elements.size();
  s->elements.push_back(elerium);
  s->floors[0].generators_bv |= (1 << e_index);
  s->floors[0].microchips_bv |= (1 << e_index);
  int d_index = s->elements.size();
  s->elements.push_back(dilithium);
  s->floors[0].generators_bv |= (1 << d_index);
  s->floors[0].microchips_bv |= (1 << d_index);
  return IntReturn(StepsToAllOnFourthFloor(*s));
}

}  // namespace advent_of_code
