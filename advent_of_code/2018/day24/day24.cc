#include "advent_of_code/2018/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/interval.h"
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
  std::string_view attack_type;
  absl::flat_hash_set<std::string_view> immune;
  absl::flat_hash_set<std::string_view> weak;

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

absl::StatusOr<std::vector<Group>> Parse(absl::Span<std::string_view> input) {
  std::vector<Group> groups;

  Group::Type cur_type = Group::kNone;
  for (std::string_view row : input) {
    Group g;
    std::string_view special;
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
        std::vector<std::string_view> pieces = absl::StrSplit(special, "; ");
        for (std::string_view p : pieces) {
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

absl::flat_hash_map<int, int> SelectTargets(
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

  absl::flat_hash_map<int, int> targets;
  absl::flat_hash_set<int> selected;
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
      if (attack_group.type == defend_group.type) continue;
      if (selected.contains(j)) continue;
      int damage = attack_group.DamageTo(defend_group);
      VLOG(3) << attack_group.id << " would deal " << damage << " to "
              << defend_group.id;
      bool replace = false;
      if (damage > max_damage) {
        replace = true;
      } else if (damage == max_damage && max_damage_group != -1) {
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

  bool change = true;
  for (int round = 0; change && CountTypes(combat) > 1; ++round) {
    if (VLOG_IS_ON(2)) {
      VLOG(2) << "Round: " << round;
      for (const Group& g : combat) {
        VLOG(3) << "  " << g;
      }
    }
    combat = RunRound(std::move(combat), &change);
  }
  return combat;
}

int ImmuneLeftAfterFightWithBoost(const std::vector<Group>& start, int boost) {
  std::vector<Group> combat = start;
  for (Group& g : combat) {
    if (g.type == Group::kImmune) g.attack += boost;
  }

  combat = Fight(combat);

  auto is_immune = [](const Group& g) { return g.type == Group::kImmune; };
  if (!absl::c_all_of(combat, is_immune)) {
    // If Infection exists, either it won or stalemated, which isn't a win for
    // Immunity.
    return 0;
  }

  return absl::c_accumulate(combat, 0,
                            [](int a, const Group& g) { return a + g.units; });
}

}  // namespace

absl::StatusOr<std::string> Day_2018_24::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Group> groups, Parse(input));

  std::vector<Group> final = Fight(groups);

  int units = 0;
  for (const Group& g : final) {
    units += g.units;
  }

  return AdventReturn(units);
}

absl::StatusOr<std::string> Day_2018_24::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Group> groups, Parse(input));

  int boost = InfiniteBinarySearch([&](int test_boost) {
    VLOG(1) << "Trying boost: " << test_boost;
    int this_units_left = ImmuneLeftAfterFightWithBoost(groups, test_boost);
    if (this_units_left > 0) {
      VLOG(1) << "  Immunity won";
      return false;
    }
    VLOG(1) << "  Infection won";
    return true;
  });
  return AdventReturn(ImmuneLeftAfterFightWithBoost(groups, boost));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/24,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_24()); });

}  // namespace advent_of_code
