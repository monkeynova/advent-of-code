// https://adventofcode.com/2015/day/21
//
// --- Day 21: RPG Simulator 20XX ---
// ----------------------------------
// 
// Little Henry Case got a new video game for Christmas. It's an RPG, and
// he's stuck on a boss. He needs to know what equipment to buy at the
// shop. He hands you the controller.
// 
// In this game, the player (you) and the enemy (the boss) take turns
// attacking. The player always goes first. Each attack reduces the
// opponent's hit points by at least 1. The first character at or below 0
// hit points loses.
// 
// Damage dealt by an attacker each turn is equal to the attacker's
// damage score minus the defender's armor score. An attacker always does
// at least 1 damage. So, if the attacker has a damage score of 8, and
// the defender has an armor score of 3, the defender loses 5 hit points.
// If the defender had an armor score of 300, the defender would still
// lose 1 hit point.
// 
// Your damage score and armor score both start at zero. They can be
// increased by buying items in exchange for gold. You start with no
// items and have as much gold as you need. Your total damage or armor is
// equal to the sum of those stats from all of your items. You have 100
// hit points.
// 
// Here is what the item shop is selling:
// 
// Weapons:    Cost  Damage  Armor
// Dagger        8     4       0
// Shortsword   10     5       0
// Warhammer    25     6       0
// Longsword    40     7       0
// Greataxe     74     8       0
// 
// Armor:      Cost  Damage  Armor
// Leather      13     0       1
// Chainmail    31     0       2
// Splintmail   53     0       3
// Bandedmail   75     0       4
// Platemail   102     0       5
// 
// Rings:      Cost  Damage  Armor
// Damage +1    25     1       0
// Damage +2    50     2       0
// Damage +3   100     3       0
// Defense +1   20     0       1
// Defense +2   40     0       2
// Defense +3   80     0       3
// 
// You must buy exactly one weapon; no dual-wielding. Armor is optional,
// but you can't use more than one. You can buy 0-2 rings (at most one
// for each hand). You must use any items you buy. The shop only has one
// of each item, so you can't buy, for example, two rings of Damage +3.
// 
// For example, suppose you have 8 hit points, 5 damage, and 5 armor, and
// that the boss has 12 hit points, 7 damage, and 2 armor:
// 
// * The player deals 5-2 = 3 damage; the boss goes down to 9 hit
// points.
// 
// * The boss deals 7-5 = 2 damage; the player goes down to 6 hit
// points.
// 
// * The player deals 5-2 = 3 damage; the boss goes down to 6 hit
// points.
// 
// * The boss deals 7-5 = 2 damage; the player goes down to 4 hit
// points.
// 
// * The player deals 5-2 = 3 damage; the boss goes down to 3 hit
// points.
// 
// * The boss deals 7-5 = 2 damage; the player goes down to 2 hit
// points.
// 
// * The player deals 5-2 = 3 damage; the boss goes down to 0 hit
// points.
// 
// In this scenario, the player wins! (Barely.)
// 
// You have 100 hit points. The boss's actual stats are in your puzzle
// input. What is the least amount of gold you can spend and still win
// the fight?
//
// --- Part Two ---
// ----------------
// 
// Turns out the shopkeeper is working with the boss, and can persuade
// you to buy whatever items he wants. The other rules still apply, and
// he still only has one of each item.
// 
// What is the most amount of gold you can spend and still lose the
// fight?


// http://adventofcode.com/2015/day/21

#include "advent_of_code/2015/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/opt_cmp.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

enum ItemType {
  kWeapon = 1,
  kArmor = 2,
  kRing = 3,
};

struct Item {
  std::string name;
  ItemType type;
  int cost;
  int damage;
  int armor;
};

// Shop items
std::vector<Item> weapons = {
    {"Dagger", kWeapon, 8, 4, 0},     {"Shortsword", kWeapon, 10, 5, 0},
    {"Warhammer", kWeapon, 25, 6, 0}, {"Longsword", kWeapon, 40, 7, 0},
    {"Greataxe", kWeapon, 74, 8, 0},
};

std::vector<Item> armors = {
    {"Leather", kArmor, 13, 0, 1},    {"Chainmail", kArmor, 31, 0, 2},
    {"Splintmail", kArmor, 53, 0, 3}, {"Bandedmail", kArmor, 75, 0, 4},
    {"Platemail", kArmor, 102, 0, 5},
};

std::vector<Item> rings = {
    {"Damage +1", kRing, 25, 1, 0},  {"Damage +2", kRing, 50, 2, 0},
    {"Damage +3", kRing, 100, 3, 0}, {"Defense +1", kRing, 20, 0, 1},
    {"Defense +2", kRing, 40, 0, 2}, {"Defense +3", kRing, 80, 0, 3},
};

struct Character {
  int hit_points;
  int damage;
  int armor;
};

bool CanWin(Character boss, Character me) {
  while (true) {
    boss.hit_points -= std::max(me.damage - boss.armor, 1);
    if (boss.hit_points <= 0) return true;
    me.hit_points -= std::max(boss.damage - me.armor, 1);
    if (me.hit_points <= 0) return false;
  }
}

absl::optional<int> MinCostToWinRings(Character boss, Character me, Item weapon,
                                      absl::optional<Item> armor,
                                      std::vector<Item> rings) {
  for (Item r : rings) {
    me.armor += r.armor;
    me.damage += r.damage;
  }
  if (!CanWin(boss, me)) return absl::nullopt;
  VLOG(1) << "Can win with " << weapon.name << ", "
          << (armor ? armor->name : "<no armor>") << ", "
          << absl::StrJoin(rings, ", ", [](std::string* out, const Item& i) {
               absl::StrAppend(out, i.name);
             });
  int cost = 0;
  cost += weapon.cost;
  if (armor) cost += armor->cost;
  for (Item r : rings) cost += r.cost;
  return cost;
}

absl::optional<int> MinCostToWinArmor(Character boss, Character me, Item weapon,
                                      absl::optional<Item> armor) {
  if (armor) {
    me.armor += armor->armor;
  }
  absl::optional<int> min;
  min = opt_min(min, MinCostToWinRings(boss, me, weapon, armor, {}));
  for (int ring_id = 0; ring_id < rings.size(); ++ring_id) {
    min = opt_min(min,
                  MinCostToWinRings(boss, me, weapon, armor, {rings[ring_id]}));
  }
  for (int ring_id1 = 0; ring_id1 < rings.size(); ++ring_id1) {
    for (int ring_id2 = 0; ring_id2 < ring_id1; ++ring_id2) {
      min = opt_min(min, MinCostToWinRings(boss, me, weapon, armor,
                                           {rings[ring_id1], rings[ring_id2]}));
    }
  }
  return min;
}

absl::optional<int> MinCostToWinWeapon(Character boss, Character me,
                                       Item weapon) {
  me.damage += weapon.damage;
  absl::optional<int> min;
  min = opt_min(min, MinCostToWinArmor(boss, me, weapon, absl::nullopt));
  for (int armor_id = 0; armor_id < armors.size(); ++armor_id) {
    min = opt_min(min, MinCostToWinArmor(boss, me, weapon, armors[armor_id]));
  }
  return min;
}

absl::optional<int> MinCostToWin(Character boss) {
  Character me{100, 0, 0};
  absl::optional<int> min;
  for (int weapon_id = 0; weapon_id < weapons.size(); ++weapon_id) {
    min = opt_min(min, MinCostToWinWeapon(boss, me, weapons[weapon_id]));
  }
  return min;
}

absl::optional<int> MaxCostToLoseRings(Character boss, Character me,
                                       Item weapon, absl::optional<Item> armor,
                                       std::vector<Item> rings) {
  for (Item r : rings) {
    me.armor += r.armor;
    me.damage += r.damage;
  }
  if (CanWin(boss, me)) return absl::nullopt;
  VLOG(1) << "Can't win with " << weapon.name << ", "
          << (armor ? armor->name : "<no armor>") << ", "
          << absl::StrJoin(rings, ", ", [](std::string* out, const Item& i) {
               absl::StrAppend(out, i.name);
             });
  int cost = 0;
  cost += weapon.cost;
  if (armor) cost += armor->cost;
  for (Item r : rings) cost += r.cost;
  return cost;
}

absl::optional<int> MaxCostToLoseArmor(Character boss, Character me,
                                       Item weapon,
                                       absl::optional<Item> armor) {
  if (armor) {
    me.armor += armor->armor;
  }
  absl::optional<int> max;
  max = opt_max(max, MaxCostToLoseRings(boss, me, weapon, armor, {}));
  for (int ring_id = 0; ring_id < rings.size(); ++ring_id) {
    max = opt_max(
        max, MaxCostToLoseRings(boss, me, weapon, armor, {rings[ring_id]}));
  }
  for (int ring_id1 = 0; ring_id1 < rings.size(); ++ring_id1) {
    for (int ring_id2 = 0; ring_id2 < ring_id1; ++ring_id2) {
      max =
          opt_max(max, MaxCostToLoseRings(boss, me, weapon, armor,
                                          {rings[ring_id1], rings[ring_id2]}));
    }
  }
  return max;
}

absl::optional<int> MaxCostToLoseWeapon(Character boss, Character me,
                                        Item weapon) {
  me.damage += weapon.damage;
  absl::optional<int> max;
  max = opt_max(max, MaxCostToLoseArmor(boss, me, weapon, absl::nullopt));
  for (int armor_id = 0; armor_id < armors.size(); ++armor_id) {
    max = opt_max(max, MaxCostToLoseArmor(boss, me, weapon, armors[armor_id]));
  }
  return max;
}

absl::optional<int> MaxCostToLose(Character boss) {
  Character me{100, 0, 0};
  absl::optional<int> max;
  for (int weapon_id = 0; weapon_id < weapons.size(); ++weapon_id) {
    max = opt_max(max, MaxCostToLoseWeapon(boss, me, weapons[weapon_id]));
  }
  return max;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_21::Part1(
    absl::Span<absl::string_view> input) const {
  Character boss{0, 0, 0};
  for (absl::string_view str : input) {
    if (!RE2::FullMatch(str, "Hit Points: (\\d+)", &boss.hit_points) &&
        !RE2::FullMatch(str, "Damage: (\\d+)", &boss.damage) &&
        !RE2::FullMatch(str, "Armor: (\\d+)", &boss.armor)) {
      return Error("Bad input: ", str);
    }
  }
  if (boss.hit_points == 0) return Error("No Hit Points");
  if (boss.damage == 0) return Error("No Damage");
  if (boss.armor == 0) return Error("No Armor");

  return IntReturn(MinCostToWin(boss));
}

absl::StatusOr<std::string> Day_2015_21::Part2(
    absl::Span<absl::string_view> input) const {
  Character boss{0, 0, 0};
  for (absl::string_view str : input) {
    if (!RE2::FullMatch(str, "Hit Points: (\\d+)", &boss.hit_points) &&
        !RE2::FullMatch(str, "Damage: (\\d+)", &boss.damage) &&
        !RE2::FullMatch(str, "Armor: (\\d+)", &boss.armor)) {
      return Error("Bad input: ", str);
    }
  }
  if (boss.hit_points == 0) return Error("No Hit Points");
  if (boss.damage == 0) return Error("No Damage");
  if (boss.armor == 0) return Error("No Armor");

  return IntReturn(MaxCostToLose(boss));
}

}  // namespace advent_of_code
