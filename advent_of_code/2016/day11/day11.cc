// https://adventofcode.com/2016/day/11
//
// --- Day 11: Radioisotope Thermoelectric Generators ---
// ------------------------------------------------------
// 
// You come upon a column of four floors that have been entirely sealed
// off from the rest of the building except for a small dedicated lobby.
// There are some radiation warnings and a big sign which reads
// "Radioisotope Testing Facility".
// 
// According to the project status board, this facility is currently
// being used to experiment with Radioisotope Thermoelectric Generators
// (RTGs, or simply "generators") that are designed to be paired with
// specially-constructed microchips. Basically, an RTG is a highly
// radioactive rock that generates electricity through heat.
// 
// The experimental RTGs have poor radiation containment, so they're
// dangerously radioactive. The chips are prototypes and don't have
// normal radiation shielding, but they do have the ability to generate
// an electromagnetic radiation shield when powered. Unfortunately, they
// can only be powered by their corresponding RTG. An RTG powering a
// microchip is still dangerous to other microchips.
// 
// In other words, if a chip is ever left in the same area as another
// RTG, and it's not connected to its own RTG, the chip will be fried.
// Therefore, it is assumed that you will follow procedure and keep chips
// connected to their corresponding RTG when they're in the same room,
// and away from other RTGs otherwise.
// 
// These microchips sound very interesting and useful to your current
// activities, and you'd like to try to retrieve them. The fourth floor
// of the facility has an assembling machine which can make a
// self-contained, shielded computer for you to take with you - that is,
// if you can bring it all of the RTGs and microchips.
// 
// Within the radiation-shielded part of the facility (in which it's safe
// to have these pre-assembly RTGs), there is an elevator that can move
// between the four floors. Its capacity rating means it can carry at
// most yourself and two RTGs or microchips in any combination. (They're
// rigged to some heavy diagnostic equipment - the assembling machine
// will detach it for you.) As a security measure, the elevator will only
// function if it contains at least one RTG or microchip. The elevator
// always stops on each floor to recharge, and this takes long enough
// that the items within it and the items on that floor can irradiate
// each other. (You can prevent this if a Microchip and its Generator end
// up on the same floor in this way, as they can be connected while the
// elevator is recharging.)
// 
// You make some notes of the locations of each component of interest
// (your puzzle input). Before you don a hazmat suit and start moving
// things around, you'd like to have an idea of what you need to do.
// 
// When you enter the containment area, you and the elevator will start
// on the first floor.
// 
// For example, suppose the isolated area has the following arrangement:
// 
// The first floor contains a hydrogen-compatible microchip and a lithium-compatible microchip.
// The second floor contains a hydrogen generator.
// The third floor contains a lithium generator.
// The fourth floor contains nothing relevant.
// 
// As a diagram (F# for a Floor number, E for Elevator, H for Hydrogen, L
// for Lithium, M for Microchip, and G for Generator), the initial state
// looks like this:
// 
// F4 .  .  .  .  .  
// F3 .  .  .  LG .  
// F2 .  HG .  .  .  
// F1 E  .  HM .  LM 
// 
// Then, to get everything up to the assembling machine on the fourth
// floor, the following steps could be taken:
// 
// * 
// 
// Bring the Hydrogen-compatible Microchip to the second floor, which
// is safe because it can get power from the Hydrogen Generator:
// 
// F4 .  .  .  .  .  
// F3 .  .  .  LG .  
// F2 E  HG HM .  .  
// F1 .  .  .  .  LM      
// 
// * 
// 
// Bring both Hydrogen-related items to the third floor, which is
// safe because the Hydrogen-compatible microchip is getting power
// from its generator:
// 
// F4 .  .  .  .  .  
// F3 E  HG HM LG .  
// F2 .  .  .  .  .  
// F1 .  .  .  .  LM      
// 
// * 
// 
// Leave the Hydrogen Generator on floor three, but bring the
// Hydrogen-compatible Microchip back down with you so you can still
// use the elevator:
// 
// F4 .  .  .  .  .  
// F3 .  HG .  LG .  
// F2 E  .  HM .  .  
// F1 .  .  .  .  LM      
// 
// * 
// 
// At the first floor, grab the Lithium-compatible Microchip, which
// is safe because Microchips don't affect each other:
// 
// F4 .  .  .  .  .  
// F3 .  HG .  LG .  
// F2 .  .  .  .  .  
// F1 E  .  HM .  LM      
// 
// * 
// 
// Bring both Microchips up one floor, where there is nothing to fry
// them:
// 
// F4 .  .  .  .  .  
// F3 .  HG .  LG .  
// F2 E  .  HM .  LM 
// F1 .  .  .  .  .       
// 
// * 
// 
// Bring both Microchips up again to floor three, where they can be
// temporarily connected to their corresponding generators while the
// elevator recharges, preventing either of them from being fried:
// 
// F4 .  .  .  .  .  
// F3 E  HG HM LG LM 
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// * 
// 
// Bring both Microchips to the fourth floor:
// 
// F4 E  .  HM .  LM 
// F3 .  HG .  LG .  
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// * 
// 
// Leave the Lithium-compatible microchip on the fourth floor, but
// bring the Hydrogen-compatible one so you can still use the
// elevator; this is safe because although the Lithium Generator is
// on the destination floor, you can connect Hydrogen-compatible
// microchip to the Hydrogen Generator there:
// 
// F4 .  .  .  .  LM 
// F3 E  HG HM LG .  
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// * 
// 
// Bring both Generators up to the fourth floor, which is safe
// because you can connect the Lithium-compatible Microchip to the
// Lithium Generator upon arrival:
// 
// F4 E  HG .  LG LM 
// F3 .  .  HM .  .  
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// * 
// 
// Bring the Lithium Microchip with you to the third floor so you can
// use the elevator:
// 
// F4 .  HG .  LG .  
// F3 E  .  HM .  LM 
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// * 
// 
// Bring both Microchips to the fourth floor:
// 
// F4 E  HG HM LG LM 
// F3 .  .  .  .  .  
// F2 .  .  .  .  .  
// F1 .  .  .  .  .       
// 
// In this arrangement, it takes 11 steps to collect all of the objects
// at the fourth floor for assembly. (Each elevator stop counts as one
// step, even if nothing is added to or removed from it.)
// 
// In your situation, what is the minimum number of steps required to
// bring all of the objects to the fourth floor?
//
// --- Part Two ---
// ----------------
// 
// You step into the cleanroom separating the lobby from the isolated
// area and put on the hazmat suit.
// 
// Upon entering the isolated containment area, however, you notice some
// extra parts on the first floor that weren't listed on the record
// outside:
// 
// * An elerium generator.
// 
// * An elerium-compatible microchip.
// 
// * A dilithium generator.
// 
// * A dilithium-compatible microchip.
// 
// These work just like the other generators and microchips. You'll have
// to get them up to assembly as well.
// 
// What is the minimum number of steps required to bring all of the
// objects, including these four new ones, to the fourth floor?


#include "advent_of_code/2016/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "absl/log/log.h"
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
  std::vector<int> Microchips() const {
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

  bool IsEmpty() const { return generators_bv_ == 0 && microchips_bv_ == 0; }

  void AddMicrochip(int microchip) { microchips_bv_ |= (1 << microchip); }

  void AddGenerator(int generator) { generators_bv_ |= (1 << generator); }

  void RemoveMicrochip(int microchip) { microchips_bv_ &= ~(1 << microchip); }

  void RemoveGenerator(int generator) { generators_bv_ &= ~(1 << generator); }

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
  std::vector<int> generators = floors[cur_floor_num].Generators();
  std::vector<int> microchips = floors[cur_floor_num].Microchips();

  // If we've cleared the lower N levels, never consider bringing anything back
  // down to them.
  int min_floor = 0;
  while (floors[min_floor].IsEmpty()) ++min_floor;

  for (int floor_delta : {-1, +1}) {
    int next_floor = cur_floor_num + floor_delta;
    if (next_floor >= floors.size()) continue;
    if (next_floor < min_floor) continue;

    for (int g1 : generators) {
      ElevatorState next = *this;
      next.MoveGenerator(cur_floor_num, next_floor, g1);
      if (!next.floors[cur_floor_num].IsValid()) continue;
      if (!next.floors[next_floor].IsValid()) continue;
      ++next.steps;
      next.cur_floor_num = next_floor;
      state->AddNextStep(next);
    }
    for (int g1 : generators) {
      for (int g2 : generators) {
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
    for (int m1 : microchips) {
      ElevatorState next = *this;
      next.MoveMicrochip(cur_floor_num, next_floor, m1);
      if (!next.floors[cur_floor_num].IsValid()) continue;
      if (!next.floors[next_floor].IsValid()) continue;
      ++next.steps;
      next.cur_floor_num = next_floor;
      state->AddNextStep(next);
    }
    for (int m1 : microchips) {
      for (int m2 : microchips) {
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
    for (int m : microchips) {
      for (int g : generators) {
        ElevatorState next = *this;
        next.MoveMicrochip(cur_floor_num, next_floor, m);
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
    // There have been two forms seen of a line with two elements.
    // Originally we saw "... contains $foo, and $bar", but a re-download of
    // the input shows a newer "... contains $foo and $bar" (no comma). We
    // attempt to parse both forms.
    if (components.back().find("and ") != std::string::npos) {
      std::vector<absl::string_view> sub_components =
          absl::StrSplit(components.back(), "and ");
      components.pop_back();
      for (absl::string_view sc : sub_components) {
        if (sc.empty()) continue;
        absl::ConsumeSuffix(&sc, " ");
        components.push_back(sc);
      }
    }
    absl::ConsumeSuffix(&components.back(), ".");
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
