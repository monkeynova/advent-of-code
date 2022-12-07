// https://adventofcode.com/2015/day/22
//
// --- Day 22: Wizard Simulator 20XX ---
// -------------------------------------
// 
// Little Henry Case decides that defeating bosses with swords and stuff
// is boring. Now he's playing the game with a wizard. Of course, he gets
// stuck on another boss and needs your help again.
// 
// In this version, combat still proceeds with the player and the boss
// taking alternating turns. The player still goes first. Now, however,
// you don't get any equipment; instead, you must choose one of your
// spells to cast. The first character at or below 0 hit points loses.
// 
// Since you're a wizard, you don't get to wear armor, and you can't
// attack normally. However, since you do magic damage, your opponent's
// armor is ignored, and so the boss effectively has zero armor as well.
// As before, if armor (from a spell, in this case) would reduce damage
// below 1, it becomes 1 instead - that is, the boss' attacks always deal
// at least 1 damage.
// 
// On each of your turns, you must select one of your spells to cast. If
// you cannot afford to cast any spell, you lose. Spells cost mana; you
// start with 500 mana, but have no maximum limit. You must have enough
// mana to cast a spell, and its cost is immediately deducted when you
// cast it. Your spells are Magic Missile, Drain, Shield, Poison, and
// Recharge.
// 
// * Magic Missile costs 53 mana. It instantly does 4 damage.
// 
// * Drain costs 73 mana. It instantly does 2 damage and heals you for
// 2 hit points.
// 
// * Shield costs 113 mana. It starts an effect that lasts for 6 turns.
// While it is active, your armor is increased by 7.
// 
// * Poison costs 173 mana. It starts an effect that lasts for 6 turns.
// At the start of each turn while it is active, it deals the boss 3
// damage.
// 
// * Recharge costs 229 mana. It starts an effect that lasts for 5
// turns. At the start of each turn while it is active, it gives you
// 101 new mana.
// 
// Effects all work the same way. Effects apply at the start of both the
// player's turns and the boss' turns. Effects are created with a timer
// (the number of turns they last); at the start of each turn, after they
// apply any effect they have, their timer is decreased by one. If this
// decreases the timer to zero, the effect ends. You cannot cast a spell
// that would start an effect which is already active. However, effects
// can be started on the same turn they end.
// 
// For example, suppose the player has 10 hit points and 250 mana, and
// that the boss has 13 hit points and 8 damage:
// 
// -- Player turn --
// - Player has 10 hit points, 0 armor, 250 mana
// - Boss has 13 hit points
// Player casts Poison.
// 
// -- Boss turn --
// - Player has 10 hit points, 0 armor, 77 mana
// - Boss has 13 hit points
// Poison deals 3 damage; its timer is now 5.
// Boss attacks for 8 damage.
// 
// -- Player turn --
// - Player has 2 hit points, 0 armor, 77 mana
// - Boss has 10 hit points
// Poison deals 3 damage; its timer is now 4.
// Player casts Magic Missile, dealing 4 damage.
// 
// -- Boss turn --
// - Player has 2 hit points, 0 armor, 24 mana
// - Boss has 3 hit points
// Poison deals 3 damage. This kills the boss, and the player wins.
// 
// Now, suppose the same initial conditions, except that the boss has 14
// hit points instead:
// 
// -- Player turn --
// - Player has 10 hit points, 0 armor, 250 mana
// - Boss has 14 hit points
// Player casts Recharge.
// 
// -- Boss turn --
// - Player has 10 hit points, 0 armor, 21 mana
// - Boss has 14 hit points
// Recharge provides 101 mana; its timer is now 4.
// Boss attacks for 8 damage!
// 
// -- Player turn --
// - Player has 2 hit points, 0 armor, 122 mana
// - Boss has 14 hit points
// Recharge provides 101 mana; its timer is now 3.
// Player casts Shield, increasing armor by 7.
// 
// -- Boss turn --
// - Player has 2 hit points, 7 armor, 110 mana
// - Boss has 14 hit points
// Shield's timer is now 5.
// Recharge provides 101 mana; its timer is now 2.
// Boss attacks for 8 - 7 = 1 damage!
// 
// -- Player turn --
// - Player has 1 hit point, 7 armor, 211 mana
// - Boss has 14 hit points
// Shield's timer is now 4.
// Recharge provides 101 mana; its timer is now 1.
// Player casts Drain, dealing 2 damage, and healing 2 hit points.
// 
// -- Boss turn --
// - Player has 3 hit points, 7 armor, 239 mana
// - Boss has 12 hit points
// Shield's timer is now 3.
// Recharge provides 101 mana; its timer is now 0.
// Recharge wears off.
// Boss attacks for 8 - 7 = 1 damage!
// 
// -- Player turn --
// - Player has 2 hit points, 7 armor, 340 mana
// - Boss has 12 hit points
// Shield's timer is now 2.
// Player casts Poison.
// 
// -- Boss turn --
// - Player has 2 hit points, 7 armor, 167 mana
// - Boss has 12 hit points
// Shield's timer is now 1.
// Poison deals 3 damage; its timer is now 5.
// Boss attacks for 8 - 7 = 1 damage!
// 
// -- Player turn --
// - Player has 1 hit point, 7 armor, 167 mana
// - Boss has 9 hit points
// Shield's timer is now 0.
// Shield wears off, decreasing armor by 7.
// Poison deals 3 damage; its timer is now 4.
// Player casts Magic Missile, dealing 4 damage.
// 
// -- Boss turn --
// - Player has 1 hit point, 0 armor, 114 mana
// - Boss has 2 hit points
// Poison deals 3 damage. This kills the boss, and the player wins.
// 
// You start with 50 hit points and 500 mana points. The boss's actual
// stats are in your puzzle input. What is the least amount of mana you
// can spend and still win the fight? (Do not include mana recharge
// effects as "spending" negative mana.)
//
// --- Part Two ---
// ----------------
// 
// On the next run through the game, you increase the difficulty to hard.
// 
// At the start of each player turn (before any other effects apply), you
// lose 1 hit point. If this brings you to or below 0 hit points, you
// lose.
// 
// With the same starting stats for you and the boss, what is the least
// amount of mana you can spend and still win the fight?

#include "advent_of_code/2015/day22/day22.h"

#include <queue>

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

struct Character {
  int hit_points;
  int damage;
  int armor;
  int mana;
};

void NoEffect(Character& me, Character& boss) {}

struct Effect {
  std::string name;
  int duration;
  std::function<void(Character&, Character&)> on_turn;
  std::function<void(Character&, Character&)> on_remove;
};

void ShieldOff(Character& me, Character& boss) { me.armor -= 7; }

void PoisonEffect(Character& me, Character& boss) { boss.hit_points -= 3; }

void RechargeEffect(Character& me, Character& boss) { me.mana += 101; }

struct GameState {
  int mana_cost;
  Character me;
  Character boss;
  absl::flat_hash_map<std::string, Effect> effects;
};

// Helper methods go here.

void ApplyEffects(absl::flat_hash_map<std::string, Effect>& active_effects,
                  Character& me, Character& boss) {
  std::vector<absl::string_view> to_remove;
  for (auto& [_, effect] : active_effects) {
    effect.on_turn(me, boss);
    if (--effect.duration == 0) {
      effect.on_remove(me, boss);
      to_remove.push_back(effect.name);
    }
  }
  for (absl::string_view name : to_remove) {
    active_effects.erase(name);
  }
}

std::vector<GameState> NextGameStates(GameState game_state) {
  std::vector<GameState> ret;
  // Try casting each spell.
  if (game_state.me.mana >= 53) {
    GameState next = game_state;
    next.mana_cost += 53;
    next.me.mana -= 53;
    next.boss.hit_points -= 4;
    ret.push_back(next);
  }
  if (game_state.me.mana >= 73) {
    GameState next = game_state;
    next.mana_cost += 73;
    next.me.mana -= 73;
    next.me.hit_points += 2;
    next.boss.hit_points -= 2;
    ret.push_back(next);
  }
  if (game_state.me.mana >= 113 && !game_state.effects.contains("shield")) {
    GameState next = game_state;
    next.mana_cost += 113;
    next.me.mana -= 113;
    Effect shield = {"shield", 6, &NoEffect, &ShieldOff};
    next.me.armor += 7;
    next.effects.emplace(shield.name, shield);
    ret.push_back(next);
  }
  if (game_state.me.mana >= 173 && !game_state.effects.contains("poison")) {
    GameState next = game_state;
    next.mana_cost += 173;
    next.me.mana -= 173;
    Effect poison = {"poison", 6, &PoisonEffect, &NoEffect};
    next.effects.emplace(poison.name, poison);
    ret.push_back(next);
  }
  if (game_state.me.mana >= 229 && !game_state.effects.contains("recharge")) {
    GameState next = game_state;
    next.mana_cost += 229;
    next.me.mana -= 229;
    Effect recharge = {"recharge", 5, &RechargeEffect, &NoEffect};
    next.effects.emplace(recharge.name, recharge);
    ret.push_back(next);
  }

  return ret;
}

struct GameStateGt {
  bool operator()(const GameState& a, const GameState& b) {
    return a.mana_cost > b.mana_cost;
  }
};

absl::optional<int> MinManaSpendForWin(Character me, Character boss) {
  std::priority_queue<GameState, std::vector<GameState>, GameStateGt> states;
  states.push({.mana_cost = 0, .me = me, .boss = boss});
  bool run_boss_turn = false;
  while (!states.empty()) {
    GameState state = states.top();
    states.pop();
    VLOG(1) << "Trying state with mana_cost: " << state.mana_cost;
    if (state.boss.hit_points <= 0) return state.mana_cost;
    if (run_boss_turn) {
      ApplyEffects(state.effects, state.me, state.boss);
      if (state.boss.hit_points <= 0) return state.mana_cost;
      state.me.hit_points -= state.boss.damage - state.me.armor;
    }
    run_boss_turn = true;
    if (state.me.hit_points > 0) {
      ApplyEffects(state.effects, state.me, state.boss);
      if (state.boss.hit_points <= 0) return state.mana_cost;
      std::vector<GameState> next_states = NextGameStates(state);
      for (GameState& ns : next_states) {
        states.push(std::move(ns));
      }
    }
  }

  return absl::nullopt;
}

absl::optional<int> MinManaSpendForWinHard(Character me, Character boss) {
  std::priority_queue<GameState, std::vector<GameState>, GameStateGt> states;
  states.push({.mana_cost = 0, .me = me, .boss = boss});
  bool run_boss_turn = false;
  while (!states.empty()) {
    GameState state = states.top();
    states.pop();
    VLOG(1) << "Trying state with mana_cost: " << state.mana_cost;
    if (state.boss.hit_points <= 0) return state.mana_cost;
    if (run_boss_turn) {
      ApplyEffects(state.effects, state.me, state.boss);
      if (state.boss.hit_points <= 0) return state.mana_cost;
      state.me.hit_points -= state.boss.damage - state.me.armor;
    }
    run_boss_turn = true;
    --state.me.hit_points;
    if (state.me.hit_points > 0) {
      ApplyEffects(state.effects, state.me, state.boss);
      if (state.boss.hit_points <= 0) return state.mana_cost;
      std::vector<GameState> next_states = NextGameStates(state);
      for (GameState& ns : next_states) {
        states.push(std::move(ns));
      }
    }
  }

  return absl::nullopt;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_22::Part1(
    absl::Span<absl::string_view> input) const {
  Character me{50, 0, 0, 500};
  Character boss{0, 0, 0, 0};
  for (absl::string_view str : input) {
    if (!RE2::FullMatch(str, "Hit Points: (\\d+)", &boss.hit_points) &&
        !RE2::FullMatch(str, "Damage: (\\d+)", &boss.damage)) {
      return Error("Bad input: ", str);
    }
  }
  if (boss.hit_points == 0) return Error("No Hit Points");
  if (boss.damage == 0) return Error("No Damage");

  return IntReturn(MinManaSpendForWin(me, boss));
}

absl::StatusOr<std::string> Day_2015_22::Part2(
    absl::Span<absl::string_view> input) const {
  Character me{50, 0, 0, 500};
  Character boss{0, 0, 0, 0};
  for (absl::string_view str : input) {
    if (!RE2::FullMatch(str, "Hit Points: (\\d+)", &boss.hit_points) &&
        !RE2::FullMatch(str, "Damage: (\\d+)", &boss.damage)) {
      return Error("Bad input: ", str);
    }
  }
  if (boss.hit_points == 0) return Error("No Hit Points");
  if (boss.damage == 0) return Error("No Damage");

  return IntReturn(MinManaSpendForWinHard(me, boss));
}

}  // namespace advent_of_code
