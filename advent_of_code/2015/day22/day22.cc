#include "advent_of_code/2015/day22/day22.h"

#include <queue>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/opt_cmp.h"
#include "glog/logging.h"
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

absl::StatusOr<std::vector<std::string>> Day22_2015::Part1(
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

absl::StatusOr<std::vector<std::string>> Day22_2015::Part2(
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
