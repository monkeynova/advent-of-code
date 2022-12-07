// https://adventofcode.com/2018/day/24
//
// --- Day 24: Immune System Simulator 20XX ---
// --------------------------------------------
// 
// After a weird buzzing noise, you appear back at the man's cottage. He
// seems relieved to see his friend, but quickly notices that the little
// reindeer caught some kind of cold while out exploring.
// 
// The portly man explains that this reindeer's immune system isn't
// similar to regular reindeer immune systems:
// 
// The immune system and the infection each have an army made up of
// several groups; each group consists of one or more identical units.
// The armies repeatedly fight until only one army has units remaining.
// 
// Units within a group all have the same hit points (amount of damage a
// unit can take before it is destroyed), attack damage (the amount of
// damage each unit deals), an attack type, an initiative (higher
// initiative units attack first and win ties), and sometimes weaknesses
// or immunities. Here is an example group:
// 
// 18 units each with 729 hit points (weak to fire; immune to cold, slashing)
// with an attack that does 8 radiation damage at initiative 10
// 
// Each group also has an effective power: the number of units in that
// group multiplied by their attack damage. The above group has an
// effective power of 18 * 8 = 144. Groups never have zero or negative
// units; instead, the group is removed from combat.
// 
// Each fight consists of two phases: target selection and attacking.
// 
// During the target selection phase, each group attempts to choose one
// target. In decreasing order of effective power, groups choose their
// targets; in a tie, the group with the higher initiative chooses first.
// The attacking group chooses to target the group in the enemy army to
// which it would deal the most damage (after accounting for weaknesses
// and immunities, but not accounting for whether the defending group has
// enough units to actually receive all of that damage).
// 
// If an attacking group is considering two defending groups to which it
// would deal equal damage, it chooses to target the defending group with
// the largest effective power; if there is still a tie, it chooses the
// defending group with the highest initiative. If it cannot deal any
// defending groups damage, it does not choose a target. Defending groups
// can only be chosen as a target by one attacking group.
// 
// At the end of the target selection phase, each group has selected zero
// or one groups to attack, and each group is being attacked by zero or
// one groups.
// 
// During the attacking phase, each group deals damage to the target it
// selected, if any. Groups attack in decreasing order of initiative,
// regardless of whether they are part of the infection or the immune
// system. (If a group contains no units, it cannot attack.)
// 
// The damage an attacking group deals to a defending group depends on
// the attacking group's attack type and the defending group's immunities
// and weaknesses. By default, an attacking group would deal damage equal
// to its effective power to the defending group. However, if the
// defending group is immune to the attacking group's attack type, the
// defending group instead takes no damage; if the defending group is
// weak to the attacking group's attack type, the defending group instead
// takes double damage.
// 
// The defending group only loses whole units from damage; damage is
// always dealt in such a way that it kills the most units possible, and
// any remaining damage to a unit that does not immediately kill it is
// ignored. For example, if a defending group contains 10 units with 10
// hit points each and receives 75 damage, it loses exactly 7 units and
// is left with 3 units at full health.
// 
// After the fight is over, if both armies still contain units, a new
// fight begins; combat only ends once one army has lost all of its
// units.
// 
// For example, consider the following armies:
// 
// Immune System:
// 17 units each with 5390 hit points (weak to radiation, bludgeoning) with
// an attack that does 4507 fire damage at initiative 2
// 989 units each with 1274 hit points (immune to fire; weak to bludgeoning,
// slashing) with an attack that does 25 slashing damage at initiative 3
// 
// Infection:
// 801 units each with 4706 hit points (weak to radiation) with an attack
// that does 116 bludgeoning damage at initiative 1
// 4485 units each with 2961 hit points (immune to radiation; weak to fire,
// cold) with an attack that does 12 slashing damage at initiative 4
// 
// If these armies were to enter combat, the following fights, including
// details during the target selection and attacking phases, would take
// place:
// 
// Immune System:
// Group 1 contains 17 units
// Group 2 contains 989 units
// Infection:
// Group 1 contains 801 units
// Group 2 contains 4485 units
// 
// Infection group 1 would deal defending group 1 185832 damage
// Infection group 1 would deal defending group 2 185832 damage
// Infection group 2 would deal defending group 2 107640 damage
// Immune System group 1 would deal defending group 1 76619 damage
// Immune System group 1 would deal defending group 2 153238 damage
// Immune System group 2 would deal defending group 1 24725 damage
// 
// Infection group 2 attacks defending group 2, killing 84 units
// Immune System group 2 attacks defending group 1, killing 4 units
// Immune System group 1 attacks defending group 2, killing 51 units
// Infection group 1 attacks defending group 1, killing 17 units
// 
// Immune System:
// Group 2 contains 905 units
// Infection:
// Group 1 contains 797 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 184904 damage
// Immune System group 2 would deal defending group 1 22625 damage
// Immune System group 2 would deal defending group 2 22625 damage
// 
// Immune System group 2 attacks defending group 1, killing 4 units
// Infection group 1 attacks defending group 2, killing 144 units
// 
// Immune System:
// Group 2 contains 761 units
// Infection:
// Group 1 contains 793 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 183976 damage
// Immune System group 2 would deal defending group 1 19025 damage
// Immune System group 2 would deal defending group 2 19025 damage
// 
// Immune System group 2 attacks defending group 1, killing 4 units
// Infection group 1 attacks defending group 2, killing 143 units
// 
// Immune System:
// Group 2 contains 618 units
// Infection:
// Group 1 contains 789 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 183048 damage
// Immune System group 2 would deal defending group 1 15450 damage
// Immune System group 2 would deal defending group 2 15450 damage
// 
// Immune System group 2 attacks defending group 1, killing 3 units
// Infection group 1 attacks defending group 2, killing 143 units
// 
// Immune System:
// Group 2 contains 475 units
// Infection:
// Group 1 contains 786 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 182352 damage
// Immune System group 2 would deal defending group 1 11875 damage
// Immune System group 2 would deal defending group 2 11875 damage
// 
// Immune System group 2 attacks defending group 1, killing 2 units
// Infection group 1 attacks defending group 2, killing 142 units
// 
// Immune System:
// Group 2 contains 333 units
// Infection:
// Group 1 contains 784 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 181888 damage
// Immune System group 2 would deal defending group 1 8325 damage
// Immune System group 2 would deal defending group 2 8325 damage
// 
// Immune System group 2 attacks defending group 1, killing 1 unit
// Infection group 1 attacks defending group 2, killing 142 units
// 
// Immune System:
// Group 2 contains 191 units
// Infection:
// Group 1 contains 783 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 181656 damage
// Immune System group 2 would deal defending group 1 4775 damage
// Immune System group 2 would deal defending group 2 4775 damage
// 
// Immune System group 2 attacks defending group 1, killing 1 unit
// Infection group 1 attacks defending group 2, killing 142 units
// 
// Immune System:
// Group 2 contains 49 units
// Infection:
// Group 1 contains 782 units
// Group 2 contains 4434 units
// 
// Infection group 1 would deal defending group 2 181424 damage
// Immune System group 2 would deal defending group 1 1225 damage
// Immune System group 2 would deal defending group 2 1225 damage
// 
// Immune System group 2 attacks defending group 1, killing 0 units
// Infection group 1 attacks defending group 2, killing 49 units
// 
// Immune System:
// No groups remain.
// Infection:
// Group 1 contains 782 units
// Group 2 contains 4434 units
// 
// In the example above, the winning army ends up with 782 + 4434 = 5216
// units.
// 
// You scan the reindeer's condition (your puzzle input); the
// white-bearded man looks nervous. As it stands now, how many units
// would the winning army have?
//
// --- Part Two ---
// ----------------
// 
// Things aren't looking good for the reindeer. The man asks whether more
// milk and cookies would help you think.
// 
// If only you could give the reindeer's immune system a boost, you might
// be able to change the outcome of the combat.
// 
// A boost is an integer increase in immune system units' attack damage.
// For example, if you were to boost the above example's immune system's
// units by 1570, the armies would instead look like this:
// 
// Immune System:
// 17 units each with 5390 hit points (weak to radiation, bludgeoning) with
// an attack that does    6077    fire damage at initiative 2
// 989 units each with 1274 hit points (immune to fire; weak to bludgeoning,
// slashing) with an attack that does    1595    slashing damage at initiative 3
// 
// Infection:
// 801 units each with 4706 hit points (weak to radiation) with an attack
// that does 116 bludgeoning damage at initiative 1
// 4485 units each with 2961 hit points (immune to radiation; weak to fire,
// cold) with an attack that does 12 slashing damage at initiative 4
// 
// With this boost, the combat proceeds differently:
// 
// Immune System:
// Group 2 contains 989 units
// Group 1 contains 17 units
// Infection:
// Group 1 contains 801 units
// Group 2 contains 4485 units
// 
// Infection group 1 would deal defending group 2 185832 damage
// Infection group 1 would deal defending group 1 185832 damage
// Infection group 2 would deal defending group 1 53820 damage
// Immune System group 2 would deal defending group 1 1577455 damage
// Immune System group 2 would deal defending group 2 1577455 damage
// Immune System group 1 would deal defending group 2 206618 damage
// 
// Infection group 2 attacks defending group 1, killing 9 units
// Immune System group 2 attacks defending group 1, killing 335 units
// Immune System group 1 attacks defending group 2, killing 32 units
// Infection group 1 attacks defending group 2, killing 84 units
// 
// Immune System:
// Group 2 contains 905 units
// Group 1 contains 8 units
// Infection:
// Group 1 contains 466 units
// Group 2 contains 4453 units
// 
// Infection group 1 would deal defending group 2 108112 damage
// Infection group 1 would deal defending group 1 108112 damage
// Infection group 2 would deal defending group 1 53436 damage
// Immune System group 2 would deal defending group 1 1443475 damage
// Immune System group 2 would deal defending group 2 1443475 damage
// Immune System group 1 would deal defending group 2 97232 damage
// 
// Infection group 2 attacks defending group 1, killing 8 units
// Immune System group 2 attacks defending group 1, killing 306 units
// Infection group 1 attacks defending group 2, killing 29 units
// 
// Immune System:
// Group 2 contains 876 units
// Infection:
// Group 2 contains 4453 units
// Group 1 contains 160 units
// 
// Infection group 2 would deal defending group 2 106872 damage
// Immune System group 2 would deal defending group 2 1397220 damage
// Immune System group 2 would deal defending group 1 1397220 damage
// 
// Infection group 2 attacks defending group 2, killing 83 units
// Immune System group 2 attacks defending group 2, killing 427 units
// 
// After a few fights...
// 
// Immune System:
// Group 2 contains 64 units
// Infection:
// Group 2 contains 214 units
// Group 1 contains 19 units
// 
// Infection group 2 would deal defending group 2 5136 damage
// Immune System group 2 would deal defending group 2 102080 damage
// Immune System group 2 would deal defending group 1 102080 damage
// 
// Infection group 2 attacks defending group 2, killing 4 units
// Immune System group 2 attacks defending group 2, killing 32 units
// 
// Immune System:
// Group 2 contains 60 units
// Infection:
// Group 1 contains 19 units
// Group 2 contains 182 units
// 
// Infection group 1 would deal defending group 2 4408 damage
// Immune System group 2 would deal defending group 1 95700 damage
// Immune System group 2 would deal defending group 2 95700 damage
// 
// Immune System group 2 attacks defending group 1, killing 19 units
// 
// Immune System:
// Group 2 contains 60 units
// Infection:
// Group 2 contains 182 units
// 
// Infection group 2 would deal defending group 2 4368 damage
// Immune System group 2 would deal defending group 2 95700 damage
// 
// Infection group 2 attacks defending group 2, killing 3 units
// Immune System group 2 attacks defending group 2, killing 30 units
// 
// After a few more fights...
// 
// Immune System:
// Group 2 contains 51 units
// Infection:
// Group 2 contains 40 units
// 
// Infection group 2 would deal defending group 2 960 damage
// Immune System group 2 would deal defending group 2 81345 damage
// 
// Infection group 2 attacks defending group 2, killing 0 units
// Immune System group 2 attacks defending group 2, killing 27 units
// 
// Immune System:
// Group 2 contains 51 units
// Infection:
// Group 2 contains 13 units
// 
// Infection group 2 would deal defending group 2 312 damage
// Immune System group 2 would deal defending group 2 81345 damage
// 
// Infection group 2 attacks defending group 2, killing 0 units
// Immune System group 2 attacks defending group 2, killing 13 units
// 
// Immune System:
// Group 2 contains 51 units
// Infection:
// No groups remain.
// 
// This boost would allow the immune system's armies to win! It would be
// left with 51 units.
// 
// You don't even know how you could boost the reindeer's immune system
// or what effect it might have, so you need to be cautious and find the
// smallest boost that would allow the immune system to win.
// 
// How many units does the immune system have left after getting the
// smallest boost it needs to win?


#include "advent_of_code/2018/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Group {
  enum Type {
    kNone = 0,
    kImmune = 1,
    kInfection = 2,
  } type;

  int id;
  int units;
  int per_unit_hit_points;
  int initiative;
  int attack;
  absl::string_view attack_type;
  absl::flat_hash_set<absl::string_view> immune;
  absl::flat_hash_set<absl::string_view> weak;

  int effective_power() const { return units * attack; }

  int DamageTo(const Group& o) const {
    if (type == o.type) return 0;
    if (o.immune.contains(attack_type)) return 0;
    if (o.weak.contains(attack_type)) return effective_power() * 2;
    return effective_power();
  }
};

std::ostream& operator<<(std::ostream& o, const Group& g) {
  return o << g.id << "{" << g.units << " of "
           << (g.type == Group::kImmune ? "immune" : "infection")
           << " hp=" << g.per_unit_hit_points << " init=" << g.initiative
           << " a=" << g.attack << " ('" << g.attack_type << "')"
           << " i={'" << absl::StrJoin(g.immune, "','") << "'}"
           << " w={'" << absl::StrJoin(g.weak, "','") << "'}"
           << "}";
}

absl::StatusOr<std::vector<Group>> Parse(absl::Span<absl::string_view> input) {
  std::vector<Group> groups;

  Group::Type cur_type = Group::kNone;
  for (absl::string_view row : input) {
    Group g;
    absl::string_view special;
    if (row.empty()) {
      cur_type = Group::kNone;
    } else if (row == "Immune System:") {
      cur_type = Group::kImmune;
    } else if (row == "Infection:") {
      cur_type = Group::kInfection;
      // 17 units each with 5390 hit points (weak to radiation, bludgeoning)
      // with an attack that does 4507 fire damage at initiative 2
    } else if (RE2::FullMatch(row,
                              "(\\d+) units each with (\\d+) hit points "
                              "\\(?(.*?)\\)? ?with an attack that does (\\d+) "
                              "(.*) damage at initiative (\\d+)",
                              &g.units, &g.per_unit_hit_points, &special,
                              &g.attack, &g.attack_type, &g.initiative)) {
      g.type = cur_type;
      g.id = groups.size();
      if (!special.empty()) {
        std::vector<absl::string_view> pieces = absl::StrSplit(special, "; ");
        for (absl::string_view p : pieces) {
          if (absl::ConsumePrefix(&p, "weak to ")) {
            g.weak = absl::StrSplit(p, ", ");
          } else if (absl::ConsumePrefix(&p, "immune to ")) {
            g.immune = absl::StrSplit(p, ", ");
          } else {
            return Error("Bad piece: ", p);
          }
        }
      }
      groups.push_back(g);
    } else {
      return Error("Bad line: ", row);
    }
  }

  return groups;
}

int CountTypes(const std::vector<Group>& groups) {
  int types[3] = {0, 0, 0};
  int seen = 0;
  for (const Group& g : groups) {
    if (++types[g.type] == 1) {
      if (++seen > 1) break;
    }
  }
  return seen;
}

const absl::flat_hash_map<int, int>& SelectTargets(
    const std::vector<Group>& groups) {
  std::vector<int> group_order(groups.size());
  std::iota(group_order.begin(), group_order.end(), 0);

  VLOG(2) << absl::StrJoin(group_order, ", ");

  std::sort(group_order.begin(), group_order.end(),
            [groups](int a_idx, int b_idx) {
              const Group& a = groups[a_idx];
              const Group& b = groups[b_idx];
              // Infection goes first.
              if (a.type != b.type) return a.type > b.type;
              if (a.effective_power() != b.effective_power()) {
                return a.effective_power() > b.effective_power();
              }
              return a.initiative > b.initiative;
            });

  static absl::flat_hash_map<int, int> targets;
  static absl::flat_hash_set<int> selected;
  targets.clear();
  selected.clear();
  for (int i : group_order) {
    const Group& attack_group = groups[i];
    VLOG(2) << "Selecting targets for " << attack_group.id << "; "
            << attack_group.effective_power();
    int max_damage = 0;
    int max_damage_group = -1;
    for (int j = 0; j < groups.size(); ++j) {
      const Group& defend_group = groups[j];
      if (selected.contains(j)) continue;
      int damage = attack_group.DamageTo(defend_group);
      VLOG(3) << attack_group.id << " would deal " << damage << " to "
              << defend_group.id;
      bool replace = false;
      if (damage > max_damage) {
        replace = true;
      } else if (damage == max_damage) {
        if (defend_group.effective_power() >
            groups[max_damage_group].effective_power()) {
          replace = true;
        } else if (defend_group.effective_power() ==
                   groups[max_damage_group].effective_power()) {
          if (defend_group.initiative > groups[max_damage_group].initiative) {
            replace = true;
          }
        }
      }
      if (replace) {
        VLOG(3) << "  Choosing";
        max_damage = damage;
        max_damage_group = j;
      }
    }
    if (max_damage > 0) {
      VLOG(2) << "  " << i << " is targeting " << max_damage_group;
      targets[i] = max_damage_group;
      selected.insert(max_damage_group);
    }
  }

  return targets;
}

std::vector<Group> RunAttack(std::vector<Group> groups,
                             const absl::flat_hash_map<int, int>& targets,
                             bool* change) {
  bool any_died = false;
  if (change != nullptr) *change = false;

  std::vector<int> group_order(groups.size());
  std::iota(group_order.begin(), group_order.end(), 0);

  std::sort(group_order.begin(), group_order.end(),
            [groups](int a_idx, int b_idx) {
              const Group& a = groups[a_idx];
              const Group& b = groups[b_idx];
              return a.initiative > b.initiative;
            });

  for (int i : group_order) {
    const Group& attack_group = groups[i];
    if (attack_group.units <= 0) continue;
    auto it = targets.find(i);
    if (it == targets.end()) continue;
    Group& defend_group = groups[it->second];
    int damage = attack_group.DamageTo(defend_group);
    VLOG(2) << "  " << attack_group.id << " deals " << damage << " to "
            << defend_group.id << " killing "
            << damage / defend_group.per_unit_hit_points;
    CHECK(damage > 0);
    int killed = damage / defend_group.per_unit_hit_points;
    if (killed > 0) {
      if (change != nullptr) *change = true;
      defend_group.units -= damage / defend_group.per_unit_hit_points;
      if (defend_group.units <= 0) any_died = true;
    }
  }

  if (any_died) {
    // There was a death. Clean it up.
    std::vector<Group> new_groups;
    new_groups.reserve(groups.size() - 1);
    for (const Group& g : groups) {
      if (g.units > 0) new_groups.push_back(g);
    }
    groups = std::move(new_groups);
  }

  return groups;
}

std::vector<Group> RunRound(std::vector<Group> groups, bool* change = nullptr) {
  absl::flat_hash_map<int, int> targets = SelectTargets(groups);

  return RunAttack(std::move(groups), targets, change);
}

std::vector<Group> Fight(const std::vector<Group>& start) {
  std::vector<Group> combat = start;

  for (int round = 0; CountTypes(combat) > 1; ++round) {
    if (VLOG_IS_ON(2)) {
      VLOG(2) << "Round: " << round;
      for (const Group& g : combat) {
        VLOG(3) << "  " << g;
      }
    }
    combat = RunRound(std::move(combat));
  }
  return combat;
}

int ImmuneLeftAfterFightWithBoost(const std::vector<Group>& start, int boost) {
  std::vector<Group> combat = start;
  for (Group& g : combat) {
    if (g.type == Group::kImmune) g.attack += boost;
  }

  bool change = true;
  for (int round = 0; change && CountTypes(combat) > 1; ++round) {
    if (VLOG_IS_ON(2)) {
      VLOG(1) << "Round: " << round;
      for (const Group& g : combat) {
        VLOG(2) << "  " << g;
      }
    }
    combat = RunRound(std::move(combat), &change);
  }

  // Stalemate isn't a win.
  if (!change) return 0;

  if (combat[0].type != Group::kImmune) return 0;

  int units = 0;
  for (const Group& g : combat) {
    units += g.units;
  }
  return units;
}

}  // namespace

absl::StatusOr<std::string> Day_2018_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Group>> groups = Parse(input);
  if (!groups.ok()) return groups.status();

  std::vector<Group> final = Fight(*groups);

  int units = 0;
  for (const Group& g : final) {
    units += g.units;
  }

  return IntReturn(units);
}

absl::StatusOr<std::string> Day_2018_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Group>> groups = Parse(input);
  if (!groups.ok()) return groups.status();

  int min = 1;
  int max = 0;
  int immunity_units_left;
  while (min != max) {
    if (max == 0) {
      int boost = 2 * min;
      VLOG(1) << "Trying boost: " << boost;
      int this_units_left = ImmuneLeftAfterFightWithBoost(*groups, boost);
      if (this_units_left > 0) {
        VLOG(1) << "  Immunity won";
        immunity_units_left = this_units_left;
        max = 2 * min;
      } else {
        VLOG(1) << "  Infection won";
        min = 2 * min;
      }
    } else {
      int boost = (max + min) / 2;
      VLOG(1) << "Trying boost: " << boost;
      int this_units_left = ImmuneLeftAfterFightWithBoost(*groups, boost);
      if (this_units_left > 0) {
        VLOG(1) << "  Immunity won";
        immunity_units_left = this_units_left;
        max = boost;
      } else {
        VLOG(1) << "  Infection won";
        min = boost + 1;
      }
    }
  }

  return IntReturn(immunity_units_left);
}

}  // namespace advent_of_code
