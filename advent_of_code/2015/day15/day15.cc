// https://adventofcode.com/2015/day/15
//
// --- Day 15: Science for Hungry People ---
// -----------------------------------------
//
// Today, you set out on the task of perfecting your milk-dunking cookie
// recipe. All you have to do is find the right balance of ingredients.
//
// Your recipe leaves room for exactly 100 teaspoons of ingredients. You
// make a list of the remaining ingredients you could use to finish the
// recipe (your puzzle input) and their properties per teaspoon:
//
// * capacity (how well it helps the cookie absorb milk)
//
// * durability (how well it keeps the cookie intact when full of milk)
//
// * flavor (how tasty it makes the cookie)
//
// * texture (how it improves the feel of the cookie)
//
// * calories (how many calories it adds to the cookie)
//
// You can only measure ingredients in whole-teaspoon amounts accurately,
// and you have to be accurate so you can reproduce your results in the
// future. The total score of a cookie can be found by adding up each of
// the properties (negative totals become 0) and then multiplying
// together everything except calories.
//
// For instance, suppose you have these two ingredients:
//
// Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8
// Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3
//
// Then, choosing to use 44 teaspoons of butterscotch and 56 teaspoons of
// cinnamon (because the amounts of each ingredient must add up to 100)
// would result in a cookie with the following properties:
//
// * A capacity of 44*-1 + 56*2 = 68
//
// * A durability of 44*-2 + 56*3 = 80
//
// * A flavor of 44*6 + 56*-2 = 152
//
// * A texture of 44*3 + 56*-1 = 76
//
// Multiplying these together (68 * 80 * 152 * 76, ignoring calories for
// now) results in a total score of 62842880, which happens to be the
// best score possible given these ingredients. If any properties had
// produced a negative total, it would have instead become zero, causing
// the whole score to multiply to zero.
//
// Given the ingredients in your kitchen and their properties, what is
// the total score of the highest-scoring cookie you can make?
//
// --- Part Two ---
// ----------------
//
// Your cookie recipe becomes wildly popular! Someone asks if you can
// make another recipe that has exactly 500 calories per cookie (so they
// can use it as a meal replacement). Keep the rest of your award-winning
// process the same (100 teaspoons, same ingredients, same scoring
// system).
//
// For example, given the ingredients above, if you had instead selected
// 40 teaspoons of butterscotch and 60 teaspoons of cinnamon (which still
// adds to 100), the total calorie count would be 40*8 + 60*3 = 500. The
// total score would go down, though: only 57600000, the best you can do
// in such trying circumstances.
//
// Given the ingredients in your kitchen and their properties, what is
// the total score of the highest-scoring cookie you can make with a
// calorie total of 500?

#include "advent_of_code/2015/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Ingredient {
  absl::string_view name;
  int64_t capacity = 0;
  int64_t durability = 0;
  int64_t flavor = 0;
  int64_t texture = 0;
  int64_t calories = 0;
};

int64_t FindBestScore(absl::Span<Ingredient> ingredients, int q_remain,
                      Ingredient score = {}) {
  VLOG(1) << "FindBestScore: " << ingredients[0].name << ": " << q_remain;
  if (ingredients.size() == 1) {
    score.capacity += ingredients[0].capacity * q_remain;
    score.durability += ingredients[0].durability * q_remain;
    score.flavor += ingredients[0].flavor * q_remain;
    score.texture += ingredients[0].texture * q_remain;
    score.calories += ingredients[0].calories * q_remain;
    int64_t ret = 1;
    ret *= score.capacity < 0 ? 0 : score.capacity;
    ret *= score.durability < 0 ? 0 : score.durability;
    ret *= score.flavor < 0 ? 0 : score.flavor;
    ret *= score.texture < 0 ? 0 : score.texture;
    // ret *= score.calories < 0 ? 0 : score.calories;
    VLOG(1) << "FindBestScore: = " << ret;
    return ret;
  }

  int64_t max = std::numeric_limits<int>::min();
  for (int i = 0; i <= q_remain; ++i) {
    if (i > 0) {
      score.capacity += ingredients[0].capacity;
      score.durability += ingredients[0].durability;
      score.flavor += ingredients[0].flavor;
      score.texture += ingredients[0].texture;
      score.calories += ingredients[0].calories;
    }
    max = std::max(max,
                   FindBestScore(ingredients.subspan(1), q_remain - i, score));
  }

  return max;
}

int64_t FindBestScoreRequiredCalories(absl::Span<Ingredient> ingredients,
                                      int q_remain, int calories,
                                      Ingredient score = {}) {
  VLOG(1) << "FindBestScore: " << ingredients[0].name << ": " << q_remain;
  if (ingredients.size() == 1) {
    score.capacity += ingredients[0].capacity * q_remain;
    score.durability += ingredients[0].durability * q_remain;
    score.flavor += ingredients[0].flavor * q_remain;
    score.texture += ingredients[0].texture * q_remain;
    score.calories += ingredients[0].calories * q_remain;
    if (score.calories != calories) return -1;
    int64_t ret = 1;
    ret *= score.capacity < 0 ? 0 : score.capacity;
    ret *= score.durability < 0 ? 0 : score.durability;
    ret *= score.flavor < 0 ? 0 : score.flavor;
    ret *= score.texture < 0 ? 0 : score.texture;
    // ret *= score.calories < 0 ? 0 : score.calories;
    VLOG(1) << "FindBestScore: = " << ret;
    return ret;
  }

  int64_t max = std::numeric_limits<int>::min();
  for (int i = 0; i <= q_remain; ++i) {
    if (i > 0) {
      score.capacity += ingredients[0].capacity;
      score.durability += ingredients[0].durability;
      score.flavor += ingredients[0].flavor;
      score.texture += ingredients[0].texture;
      score.calories += ingredients[0].calories;
    }
    max = std::max(
        max, FindBestScoreRequiredCalories(ingredients.subspan(1), q_remain - i,
                                           calories, score));
  }

  return max;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_15::Part1(
    absl::Span<absl::string_view> input) const {
  // Sprinkles: capacity 5, durability -1, flavor 0, texture 0, calories 5
  std::vector<Ingredient> ingredients;
  for (absl::string_view str : input) {
    Ingredient i;
    if (!RE2::FullMatch(str,
                        "(.*): capacity (-?\\d+), durability (-?\\d+), flavor "
                        "(-?\\d+), texture (-?\\d+), calories (-?\\d+)",
                        &i.name, &i.capacity, &i.durability, &i.flavor,
                        &i.texture, &i.calories)) {
      return Error("Bad input: ", str);
    }
    ingredients.push_back(i);
  }

  return IntReturn(FindBestScore(absl::MakeSpan(ingredients), 100));
}

absl::StatusOr<std::string> Day_2015_15::Part2(
    absl::Span<absl::string_view> input) const {
  // Sprinkles: capacity 5, durability -1, flavor 0, texture 0, calories 5
  std::vector<Ingredient> ingredients;
  for (absl::string_view str : input) {
    Ingredient i;
    if (!RE2::FullMatch(str,
                        "(.*): capacity (-?\\d+), durability (-?\\d+), flavor "
                        "(-?\\d+), texture (-?\\d+), calories (-?\\d+)",
                        &i.name, &i.capacity, &i.durability, &i.flavor,
                        &i.texture, &i.calories)) {
      return Error("Bad input: ", str);
    }
    ingredients.push_back(i);
  }

  return IntReturn(
      FindBestScoreRequiredCalories(absl::MakeSpan(ingredients), 100, 500));
}

}  // namespace advent_of_code
