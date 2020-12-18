#include "advent_of_code/2015/day15/day15.h"

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

absl::StatusOr<std::vector<std::string>> Day15_2015::Part1(
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

absl::StatusOr<std::vector<std::string>> Day15_2015::Part2(
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
