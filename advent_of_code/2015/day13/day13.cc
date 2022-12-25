#include "advent_of_code/2015/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/opt_cmp.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct CostPair {
  absl::string_view from;
  absl::string_view to;
  int happiness;
};

using CostMap =
    absl::flat_hash_map<absl::string_view,
                        absl::flat_hash_map<absl::string_view, CostPair>>;

absl::StatusOr<int> PairDelta(const CostMap& cost_map, absl::string_view from,
                              absl::string_view to) {
  auto it1 = cost_map.find(from);
  if (it1 == cost_map.end()) return Error("Can't find: ", from);
  auto it2 = it1->second.find(to);
  if (it2 == it1->second.end()) return Error("Can't find: ", to);

  auto it3 = cost_map.find(to);
  if (it3 == cost_map.end()) return Error("Can't find: ", to);
  auto it4 = it3->second.find(from);
  if (it4 == it3->second.end()) return Error("Can't find: ", from);

  VLOG(2) << "  PairDelta(" << from << ", " << to
          << "): " << it2->second.happiness + it4->second.happiness;
  return it2->second.happiness + it4->second.happiness;
}

absl::StatusOr<int> BestFillRemainingSeats(
    const CostMap& cost_map, int delta_happiness,
    std::vector<absl::string_view>* seating,
    absl::flat_hash_set<absl::string_view>* seated) {
  VLOG(1) << "BestFillRemainingSeats(" << absl::StrJoin(*seating, ",")
          << "): " << delta_happiness;

  if (seated->size() == cost_map.size()) {
    absl::StatusOr<int> wrap_around =
        PairDelta(cost_map, (*seating)[0], seating->back());
    if (!wrap_around.ok()) return wrap_around.status();
    return *wrap_around + delta_happiness;
  }

  absl::optional<int> max_happiness;
  for (const auto& to_seat_pair : cost_map) {
    absl::string_view to_seat = to_seat_pair.first;
    if (seated->contains(to_seat)) continue;

    int this_delta = 0;
    if (!seating->empty()) {
      absl::StatusOr<int> pair_delta =
          PairDelta(cost_map, to_seat, seating->back());
      if (!pair_delta.ok()) return pair_delta.status();
      this_delta = *pair_delta;
    }

    seating->push_back(to_seat);
    seated->insert(to_seat);

    absl::StatusOr<int> next_best = BestFillRemainingSeats(
        cost_map, delta_happiness + this_delta, seating, seated);
    if (!next_best.ok()) return next_best.status();
    max_happiness = opt_max(max_happiness, *next_best);

    seated->erase(to_seat);
    seating->pop_back();
  }

  if (!max_happiness) return Error("Not found");
  return *max_happiness;
}

absl::StatusOr<int> FindBestSeating(const CostMap& cost_map) {
  std::vector<absl::string_view> seating;
  absl::flat_hash_set<absl::string_view> seated;
  return BestFillRemainingSeats(cost_map, 0, &seating, &seated);
}

}  // namespace

absl::StatusOr<std::string> Day_2015_13::Part1(
    absl::Span<absl::string_view> input) const {
  // Alice would gain 2 happiness units by sitting next to Bob.
  CostMap cost_map;
  for (absl::string_view str : input) {
    CostPair cost_pair;
    absl::string_view dir;
    if (!RE2::FullMatch(str,
                        "(.*) would (gain|lose) (\\d+) happiness units by "
                        "sitting next to (.*).",
                        &cost_pair.from, &dir, &cost_pair.happiness,
                        &cost_pair.to)) {
      return Error("Bad input: ", str);
    }
    if (dir == "lose") {
      cost_pair.happiness = -cost_pair.happiness;
    }
    cost_map[cost_pair.from][cost_pair.to] = cost_pair;
  }

  return AdventReturn(FindBestSeating(cost_map));
}

absl::StatusOr<std::string> Day_2015_13::Part2(
    absl::Span<absl::string_view> input) const {
  // Alice would gain 2 happiness units by sitting next to Bob.
  CostMap cost_map;
  for (absl::string_view str : input) {
    CostPair cost_pair;
    absl::string_view dir;
    if (!RE2::FullMatch(str,
                        "(.*) would (gain|lose) (\\d+) happiness units by "
                        "sitting next to (.*).",
                        &cost_pair.from, &dir, &cost_pair.happiness,
                        &cost_pair.to)) {
      return Error("Bad input: ", str);
    }
    if (dir == "lose") {
      cost_pair.happiness = -cost_pair.happiness;
    }
    cost_map[cost_pair.from][cost_pair.to] = cost_pair;
  }
  for (const auto& pair : cost_map) {
    absl::string_view who = pair.first;
    CostPair cost_pair_from;
    cost_pair_from.from = "me";
    cost_pair_from.to = who;
    cost_pair_from.happiness = 0;
    cost_map[cost_pair_from.from][cost_pair_from.to] = cost_pair_from;
    CostPair cost_pair_to;
    cost_pair_to.from = who;
    ;
    cost_pair_to.to = "me";
    cost_pair_to.happiness = 0;
    cost_map[cost_pair_to.from][cost_pair_to.to] = cost_pair_to;
  }

  return AdventReturn(FindBestSeating(cost_map));
}

}  // namespace advent_of_code
