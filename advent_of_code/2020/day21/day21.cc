// https://adventofcode.com/2020/day/21
//
// --- Day 21: Allergen Assessment ---
// -----------------------------------
// 
// You reach the train's last stop and the closest you can get to your
// vacation island without getting wet. There aren't even any boats here,
// but nothing can stop you now: you build a raft. You just need a few
// days' worth of food for your journey.
// 
// You don't speak the local language, so you can't read any ingredients
// lists. However, sometimes, allergens are listed in a language you do
// understand. You should be able to use this information to determine
// which ingredient contains which allergen and work out which foods are
// safe to take with you on your trip.
// 
// You start by compiling a list of foods (your puzzle input), one food
// per line. Each line includes that food's ingredients list followed by
// some or all of the allergens the food contains.
// 
// Each allergen is found in exactly one ingredient. Each ingredient
// contains zero or one allergen. Allergens aren't always marked; when
// they're listed (as in (contains nuts, shellfish) after an ingredients
// list), the ingredient that contains each listed allergen will be
// somewhere in the corresponding ingredients list. However, even if an
// allergen isn't listed, the ingredient that contains that allergen
// could still be present: maybe they forgot to label it, or maybe it was
// labeled in a language you don't know.
// 
// For example, consider the following list of foods:
// 
// mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
// trh fvjkl sbzzf mxmxvkd (contains dairy)
// sqjhc fvjkl (contains soy)
// sqjhc mxmxvkd sbzzf (contains fish)
// 
// The first food in the list has four ingredients (written in a language
// you don't understand): mxmxvkd, kfcds, sqjhc, and nhms. While the food
// might contain other allergens, a few allergens the food definitely
// contains are listed afterward: dairy and fish.
// 
// The first step is to determine which ingredients can't possibly
// contain any of the allergens in any food in your list. In the above
// example, none of the ingredients kfcds, nhms, sbzzf, or trh can
// contain an allergen. Counting the number of times any of these
// ingredients appear in any ingredients list produces 5: they all appear
// once each except sbzzf, which appears twice.
// 
// Determine which ingredients cannot possibly contain any of the
// allergens in your list. How many times do any of those ingredients
// appear?
//
// --- Part Two ---
// ----------------
// 
// Now that you've isolated the inert ingredients, you should have enough
// information to figure out which ingredient contains which allergen.
// 
// In the above example:
// 
// * mxmxvkd contains dairy.
// 
// * sqjhc contains fish.
// 
// * fvjkl contains soy.
// 
// Arrange the ingredients alphabetically by their allergen and separate
// them by commas to produce your canonical dangerous ingredient list.
// (There should not be any spaces in your canonical dangerous ingredient
// list.) In the above example, this would be mxmxvkd,sqjhc,fvjkl.
// 
// Time to stock your raft with supplies. What is your canonical
// dangerous ingredient list?


// http://adventofcode.com/2020/day/21

#include "advent_of_code/2020/day21/day21.h"

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

// Helper methods go here.
struct Food {
  absl::flat_hash_set<absl::string_view> ingredients;
  absl::flat_hash_set<absl::string_view> allergens;
};

absl::StatusOr<Food> ParseFood(absl::string_view txt) {
  if (txt[txt.size() - 1] != ')') {
    return Error("Bad input(2): ", txt);
  }
  txt = txt.substr(0, txt.size() - 1);

  Food food;
  const auto [ingredients, allergen_list] = PairSplit(txt, " (contains ");

  food.ingredients = absl::StrSplit(ingredients, " ");
  food.allergens = absl::StrSplit(allergen_list, ", ");
  VLOG(2) << "Parsed: {{" << absl::StrJoin(food.ingredients, ",") << "},{"
          << absl::StrJoin(food.allergens, ",") << "}}";
  return food;
}

void Intersect(absl::flat_hash_set<absl::string_view>* out,
               const absl::flat_hash_set<absl::string_view>& in,
               const absl::flat_hash_set<absl::string_view>& not_list) {
  absl::flat_hash_set<absl::string_view> new_out;
  for (absl::string_view tmp : *out) {
    if (in.contains(tmp) && !not_list.contains(tmp)) new_out.insert(tmp);
  }
  *out = std::move(new_out);
}

absl::StatusOr<absl::flat_hash_map<absl::string_view, absl::string_view>>
FindAllergens(const std::vector<Food>& foods) {
  absl::flat_hash_map<absl::string_view, std::vector<int>> allergen_to_food_idx;
  for (int food_idx = 0; food_idx < foods.size(); ++food_idx) {
    for (absl::string_view a : foods[food_idx].allergens) {
      allergen_to_food_idx[a].push_back(food_idx);
    }
  }

  absl::flat_hash_map<absl::string_view, absl::string_view>
      allergen_to_ingredient;
  absl::flat_hash_map<absl::string_view, absl::string_view>
      ingredient_to_allergen;
  absl::flat_hash_set<absl::string_view> assigned_ingredients;

  while (allergen_to_ingredient.size() < allergen_to_food_idx.size()) {
    bool assigned = false;
    for (const auto& [allergen, food_idx_list] : allergen_to_food_idx) {
      if (allergen_to_ingredient.contains(allergen)) continue;

      absl::flat_hash_set<absl::string_view> in =
          foods[*food_idx_list.begin()].ingredients;
      for (int idx : food_idx_list) {
        Intersect(&in, foods[idx].ingredients, assigned_ingredients);
      }
      if (in.empty()) return Error("No assignment for ", allergen);
      if (in.size() == 1) {
        absl::string_view food = *in.begin();
        allergen_to_ingredient[allergen] = food;
        ingredient_to_allergen[food] = allergen;
        assigned_ingredients.insert(food);
        assigned = true;
      }
    }
    if (!assigned) return Error("Can't find an assignment");
  }

  return ingredient_to_allergen;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_21::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Food> foods;
  for (absl::string_view in : input) {
    absl::StatusOr<Food> food = ParseFood(in);
    if (!food.ok()) return food.status();
    foods.push_back(std::move(*food));
  }
  absl::StatusOr<absl::flat_hash_map<absl::string_view, absl::string_view>>
      ingredient_to_allergen = FindAllergens(foods);
  if (!ingredient_to_allergen.ok()) return ingredient_to_allergen.status();

  int count = 0;
  for (const Food& f : foods) {
    for (absl::string_view ingredient : f.ingredients) {
      if (!ingredient_to_allergen->contains(ingredient)) ++count;
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2020_21::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Food> foods;
  for (absl::string_view in : input) {
    absl::StatusOr<Food> food = ParseFood(in);
    if (!food.ok()) return food.status();
    foods.push_back(std::move(*food));
  }
  absl::StatusOr<absl::flat_hash_map<absl::string_view, absl::string_view>>
      ingredient_to_allergen = FindAllergens(foods);
  if (!ingredient_to_allergen.ok()) return ingredient_to_allergen.status();

  struct IandA {
    absl::string_view i;
    absl::string_view a;
    bool operator<(const IandA& o) const { return a < o.a; }
  };
  std::vector<IandA> ingredient_and_allergen;
  for (const auto& pair : *ingredient_to_allergen) {
    ingredient_and_allergen.push_back({.i = pair.first, .a = pair.second});
  }
  std::sort(ingredient_and_allergen.begin(), ingredient_and_allergen.end());
  std::string out = absl::StrJoin(ingredient_and_allergen, ",",
                                  [](std::string* out, const IandA& ianda) {
                                    absl::StrAppend(out, ianda.i);
                                  });
  return out;
}

}  // namespace advent_of_code
