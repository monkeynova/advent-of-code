#include "advent_of_code/2016/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
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

class ElevatorState : public BFSInterface<ElevatorState> {
 public:
  static absl::StatusOr<ElevatorState> Parse(
      absl::Span<absl::string_view> input);

  const ElevatorState& identifier() const override { return *this; }

  void AddElementAtFloor0(absl::string_view element_name) {
    int e_index = elements.size();
    elements.push_back(element_name);
    floors[0].generators_bv |= (1 << e_index);
    floors[0].microchips_bv |= (1 << e_index);
  }

  int min_steps_to_final() const override {
    // As of 2020.12.29, the AStar version which uses this method is actually
    // a bit slower, so it appears that the path pruning isn't big enough to
    // overcome the costs of the priority queue.
    if (min_steps_to_final_ == -1) {
      min_steps_to_final_ = 0;
      int floor_items = 0;
      for (int i = 0; i < floors.size() - 1; ++i) {
        int or_bv = floors[i].generators_bv | floors[i].microchips_bv;
        for (int bv = 1; (1 << bv) <= or_bv; ++bv) {
          if (floors[i].generators_bv & (1 << bv)) {
            ++floor_items;
          }
          if (floors[i].microchips_bv & (1 << bv)) {
            ++floor_items;
          }
        }
        // To move floor_items up one floor we have to move up two and down
        // with one to come back.
        min_steps_to_final_ += std::max(floor_items - 1, 0);
      }
    }
    return min_steps_to_final_;
  }

  void AddNextSteps(State* s) override;

  bool IsValid() const {
    for (const Floor& f : floors) {
      if (f.generators_bv != 0) {
        if (f.microchips_bv & ~f.generators_bv) return false;
      }
    }
    return true;
  }

  bool IsFinal() override {
    for (int idx = 0; idx < floors.size() - 1; ++idx) {
      if (floors[idx].microchips_bv != 0) return false;
      if (floors[idx].generators_bv != 0) return false;
    }
    return true;
  }

  bool operator==(const ElevatorState& o) const {
    return cur_floor == o.cur_floor && floors == o.floors;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ElevatorState& s) {
    return H::combine(std::move(h), s.cur_floor, s.floors);
  }

  friend std::ostream& operator<<(std::ostream& out, const ElevatorState& s);

 private:
  int cur_floor = 0;
  int steps = 0;
  std::vector<Floor> floors;
  std::vector<absl::string_view> elements;
  // Memoized value for min_steps_to_final().
  mutable int min_steps_to_final_ = -1;
};

std::ostream& operator<<(std::ostream& out, const ElevatorState& s) {
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

void ElevatorState::AddNextSteps(State* state) {
  int cur_floor_num = cur_floor;
  std::vector<int> next_floors;
  if (cur_floor_num - 1 >= 0) next_floors.push_back(cur_floor_num - 1);
  if (cur_floor_num + 1 < floors.size())
    next_floors.push_back(cur_floor_num + 1);
  const Floor& cur_floor = floors[cur_floor_num];

  for (int bit_index = 0; (1 << bit_index) <= cur_floor.generators_bv;
       ++bit_index) {
    if (!(cur_floor.generators_bv & (1 << bit_index))) continue;
    for (int next_floor : next_floors) {
      ElevatorState next = *this;
      ++next.steps;
      next.cur_floor = next_floor;
      next.floors[next_floor].generators_bv |= (1 << bit_index);
      next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index);
      if (next.IsValid()) state->AddNextStep(next);
    }
  }
  for (int bit_index1 = 0; (1 << bit_index1) <= cur_floor.generators_bv;
       ++bit_index1) {
    if (!(cur_floor.generators_bv & (1 << bit_index1))) continue;
    for (int bit_index2 = 0; bit_index2 < bit_index1; ++bit_index2) {
      if (!(cur_floor.generators_bv & (1 << bit_index2))) continue;
      for (int next_floor : next_floors) {
        ElevatorState next = *this;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].generators_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index1);
        next.floors[next_floor].generators_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index2);
        if (next.IsValid()) state->AddNextStep(next);
      }
    }
  }
  for (int bit_index = 0; (1 << bit_index) <= cur_floor.microchips_bv;
       ++bit_index) {
    if (!(cur_floor.microchips_bv & (1 << bit_index))) continue;
    for (int next_floor : next_floors) {
      ElevatorState next = *this;
      ++next.steps;
      next.cur_floor = next_floor;
      next.floors[next_floor].microchips_bv |= (1 << bit_index);
      next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index);
      if (next.IsValid()) state->AddNextStep(next);
    }
  }
  for (int bit_index1 = 0; (1 << bit_index1) <= cur_floor.microchips_bv;
       ++bit_index1) {
    if (!(cur_floor.microchips_bv & (1 << bit_index1))) continue;
    for (int bit_index2 = 0; bit_index2 < bit_index1; ++bit_index2) {
      if (!(cur_floor.microchips_bv & (1 << bit_index2))) continue;
      for (int next_floor : next_floors) {
        ElevatorState next = *this;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].microchips_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index1);
        next.floors[next_floor].microchips_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index2);
        if (next.IsValid()) state->AddNextStep(next);
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
        ElevatorState next = *this;
        ++next.steps;
        next.cur_floor = next_floor;
        next.floors[next_floor].microchips_bv |= (1 << bit_index1);
        next.floors[cur_floor_num].microchips_bv &= ~(1 << bit_index1);
        next.floors[next_floor].generators_bv |= (1 << bit_index2);
        next.floors[cur_floor_num].generators_bv &= ~(1 << bit_index2);
        if (next.IsValid()) state->AddNextStep(next);
      }
    }
  }
}

absl::StatusOr<ElevatorState> ElevatorState::Parse(
    absl::Span<absl::string_view> input) {
  if (input.size() != 4) return AdventDay::Error("Bad input size");
  ElevatorState s;
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
  absl::StatusOr<ElevatorState> s = ElevatorState::Parse(input);
  if (!s.ok()) return s.status();
  return IntReturn(s->FindMinSteps());
}

absl::StatusOr<std::vector<std::string>> Day11_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<ElevatorState> s = ElevatorState::Parse(input);
  s->AddElementAtFloor0("elerium");
  s->AddElementAtFloor0("dilithium");

  return IntReturn(s->FindMinSteps());
}

}  // namespace advent_of_code
