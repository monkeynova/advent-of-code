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

class Floor {
 public:
  Floor() = default;

  std::vector<int> Generators() const {
    std::vector<int> generators;
    for (int bit_index = 0; (1 << bit_index) <= generators_bv_; ++bit_index) {
      if (!(generators_bv_ & (1 << bit_index))) continue;
      generators.push_back(bit_index);
    }
    return generators;
  }
  std::vector<int>  Microchips() const {
    std::vector<int> microchips;
    for (int bit_index = 0; (1 << bit_index) <= microchips_bv_; ++bit_index) {
      if (!(microchips_bv_ & (1 << bit_index))) continue;
      microchips.push_back(bit_index);
    }
    return microchips;
  }

  bool IsValid() const {
    if (generators_bv_ == 0) return true;
    if (microchips_bv_ & ~generators_bv_) return false;
    return true;
  }

  bool IsEmpty() const {
    return generators_bv_ == 0 && microchips_bv_ == 0;
  }

  void AddMicrochip(int microchip) {
    microchips_bv_ |= (1 << microchip);
  }

  void AddGenerator(int generator) {
    generators_bv_ |= (1 << generator);
  }

  void RemoveMicrochip(int microchip) {
    microchips_bv_ &= ~(1 << microchip);
  }

  void RemoveGenerator(int generator) {
    generators_bv_ &= ~(1 << generator);
  }

  bool operator==(const Floor& o) const {
    return generators_bv_ == o.generators_bv_ && 
        microchips_bv_ == o.microchips_bv_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const Floor& f) {
    return H::combine(std::move(h), f.generators_bv_, f.microchips_bv_);
  }

 private:
  char generators_bv_ = 0;
  char microchips_bv_ = 0;
};


class ElevatorState : public BFSInterface<ElevatorState> {
 public:
  static absl::StatusOr<ElevatorState> Parse(
      absl::Span<absl::string_view> input,
      std::vector<absl::string_view>* elements);

  const ElevatorState& identifier() const override { return *this; }

  void AddElementAtFloor0(absl::string_view element_name) {
    int e_index = elements->size();
    elements->push_back(element_name);
    floors[0].AddGenerator(e_index);
    floors[0].AddMicrochip(e_index);
  }

  // As of 2020.12.29, the AStar version which uses this method is actually
  // a bit slower, so it appears that the path pruning isn't big enough to
  // overcome the costs of the priority queue.
  // int min_steps_to_final() const override;

  void AddNextSteps(State* s) const override;

  bool IsValid() const {
    for (const Floor& f : floors) {
      if (!f.IsValid()) return false;
    }
    return true;
  }

  bool IsFinal() const override {
    for (int idx = 0; idx < floors.size() - 1; ++idx) {
      if (!floors[idx].IsEmpty()) return false;
    }
    return true;
  }

  bool operator==(const ElevatorState& o) const {
    return cur_floor_num == o.cur_floor_num && floors == o.floors;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ElevatorState& s) {
    return H::combine(std::move(h), s.cur_floor_num, s.floors);
  }

  friend std::ostream& operator<<(std::ostream& out, const ElevatorState& s);

 private:
  void MoveGenerator(int from_floor, int to_floor, int generator);
  void MoveMicrochip(int from_floor, int to_floor, int microchip);

  int cur_floor_num = 0;
  int steps = 0;
  std::array<Floor, 4> floors;
  std::vector<absl::string_view>* elements;
};

std::ostream& operator<<(std::ostream& out, const ElevatorState& s) {
  for (int i = 0; i < s.floors.size(); ++i) {
    out << "F" << i + 1 << (s.cur_floor_num == i ? " E" : "  ") << ":";
    out << " G{";
    for (int g : s.floors[i].Generators()) {
      out << (*s.elements)[g] << ",";
    }
    out << "}";
    out << " M{";
    for (int m : s.floors[i].Microchips()) {
      out << (*s.elements)[m] << ",";
    }
    out << "}\n";
  }
  return out;
}

void ElevatorState::MoveGenerator(int from_floor, int to_floor, int generator) {
  floors[to_floor].AddGenerator(generator);
  floors[from_floor].RemoveGenerator(generator);
}

void ElevatorState::MoveMicrochip(int from_floor, int to_floor, int microchip) {
  floors[to_floor].AddMicrochip(microchip);
  floors[from_floor].RemoveMicrochip(microchip);  
}

void ElevatorState::AddNextSteps(State* state) const {
  std::vector<int> next_floors;
  if (cur_floor_num - 1 >= 0) next_floors.push_back(cur_floor_num - 1);
  if (cur_floor_num + 1 < floors.size()) {
    next_floors.push_back(cur_floor_num + 1);
  }
  const Floor& cur_floor = floors[cur_floor_num];

  std::vector<int> generators = cur_floor.Generators();
  std::vector<int> microchips = cur_floor.Microchips();

  for (int g1 : cur_floor.Generators()) {
    for (int next_floor : next_floors) {
      ElevatorState next = *this;
      next.MoveGenerator(cur_floor_num, next_floor, g1);
      if (!next.floors[cur_floor_num].IsValid()) continue;
      if (!next.floors[next_floor].IsValid()) continue;
      ++next.steps;
      next.cur_floor_num = next_floor;
      state->AddNextStep(next);
    }
    for (int g2 : generators) {
      for (int next_floor : next_floors) {
        ElevatorState next = *this;
        next.MoveGenerator(cur_floor_num, next_floor, g1);
        next.MoveGenerator(cur_floor_num, next_floor, g2);
        if (!next.floors[cur_floor_num].IsValid()) continue;
        if (!next.floors[next_floor].IsValid()) continue;
        ++next.steps;
        next.cur_floor_num = next_floor;
        state->AddNextStep(next);
      }
    }
  }
  for (int m1 : microchips) {
    for (int next_floor : next_floors) {
      ElevatorState next = *this;
      next.MoveMicrochip(cur_floor_num, next_floor, m1);
      if (!next.floors[cur_floor_num].IsValid()) continue;
      if (!next.floors[next_floor].IsValid()) continue;
      ++next.steps;
      next.cur_floor_num = next_floor;
      state->AddNextStep(next);
    }
    for (int m2 : microchips) {
      for (int next_floor : next_floors) {
        ElevatorState next = *this;
        next.MoveMicrochip(cur_floor_num, next_floor, m1);
        next.MoveMicrochip(cur_floor_num, next_floor, m2);
        if (!next.floors[cur_floor_num].IsValid()) continue;
        if (!next.floors[next_floor].IsValid()) continue;
        ++next.steps;
        next.cur_floor_num = next_floor;
        state->AddNextStep(next);
      }
    }
    for (int g : generators) {
      for (int next_floor : next_floors) {
        ElevatorState next = *this;
        next.MoveMicrochip(cur_floor_num, next_floor, m1);
        next.MoveGenerator(cur_floor_num, next_floor, g);
        if (!next.floors[cur_floor_num].IsValid()) continue;
        if (!next.floors[next_floor].IsValid()) continue;
        ++next.steps;
        next.cur_floor_num = next_floor;
        state->AddNextStep(next);
      }
    }
  }
}

absl::StatusOr<ElevatorState> ElevatorState::Parse(
    absl::Span<absl::string_view> input,
    std::vector<absl::string_view>* elements) {
  if (input.size() != 4) return Error("Bad input size");
  ElevatorState s;
  s.elements = elements;
  int floor = 0;
  absl::string_view kFloorNames[] = {"first", "second", "third", "fourth"};
  absl::flat_hash_map<absl::string_view, int> element_to_id;
  for (absl::string_view in : input) {
    std::string prefix =
        absl::StrCat("The ", kFloorNames[floor], " floor contains ");
    if (in.substr(0, prefix.size()) != prefix) {
      return Error("Bad prefix: ", in, " !~ ", prefix);
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
          element_to_id[e] = s.elements->size();
          s.elements->push_back(e);
        }
        s.floors[floor].AddGenerator(element_to_id[e]);
      } else if (RE2::FullMatch(comp, "a (.*)-compatible microchip", &e)) {
        if (!element_to_id.contains(e)) {
          element_to_id[e] = s.elements->size();
          s.elements->push_back(e);
        }
        s.floors[floor].AddMicrochip(element_to_id[e]);
      } else {
        return Error("Bad component: ", comp);
      }
    }
    ++floor;
  }
  return s;
}

}  // namespace

absl::StatusOr<std::string> Day_2016_11::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::string_view> element_names;
  absl::StatusOr<ElevatorState> s = ElevatorState::Parse(input, &element_names);
  if (!s.ok()) return s.status();
  return IntReturn(s->FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_11::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::string_view> element_names;
  absl::StatusOr<ElevatorState> s = ElevatorState::Parse(input, &element_names);
  s->AddElementAtFloor0("elerium");
  s->AddElementAtFloor0("dilithium");

  return IntReturn(s->FindMinSteps());
}

}  // namespace advent_of_code
