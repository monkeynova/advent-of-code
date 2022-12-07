// https://adventofcode.com/2015/day/9
//
// --- Day 9: All in a Single Night ---
// ------------------------------------
// 
// Every year, Santa manages to deliver all of his presents in a single
// night.
// 
// This year, however, he has some new locations to visit; his elves have
// provided him the distances between every pair of locations. He can
// start and end at any two (different) locations he wants, but he must
// visit each location exactly once. What is the shortest distance he can
// travel to achieve this?
// 
// For example, given the following distances:
// 
// London to Dublin = 464
// London to Belfast = 518
// Dublin to Belfast = 141
// 
// The possible routes are therefore:
// 
// Dublin -> London -> Belfast = 982
// London -> Dublin -> Belfast = 605
// London -> Belfast -> Dublin = 659
// Dublin -> Belfast -> London = 659
// Belfast -> Dublin -> London = 605
// Belfast -> London -> Dublin = 982
// 
// The shortest of these is London -> Dublin -> Belfast = 605, and so the
// answer is 605 in this example.
// 
// What is the distance of the shortest route?
//
// --- Part Two ---
// ----------------
// 
// The next year, just to show off, Santa decides to take the route with
// the longest distance instead.
// 
// He can still start and end at any two (different) locations he wants,
// and he still must visit each location exactly once.
// 
// For example, given the distances above, the longest route would be 982
// via (for example) Dublin -> London -> Belfast.
// 
// What is the distance of the longest route?


#include "advent_of_code/2015/day09/day09.h"

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

struct Route {
  absl::string_view src;
  absl::string_view dst;
  int weight;
};

absl::optional<int> ShortestAllVisitFrom(
    const absl::flat_hash_map<absl::string_view, std::vector<Route>>&
        src_routes,
    absl::string_view from, absl::flat_hash_set<absl::string_view>* visited) {
  std::string prefix;
  prefix.resize(visited->size() * 2 - 2, ' ');
  VLOG(1) << prefix << "ShortestAllVisitFrom(" << from << ", "
          << visited->size() << ")";
  if (visited->size() == src_routes.size()) return 0;
  absl::optional<int> min;
  auto it = src_routes.find(from);
  if (it == src_routes.end()) return absl::nullopt;

  for (const Route& r : it->second) {
    if (visited->contains(r.dst)) continue;
    visited->insert(r.dst);
    min = opt_min(min, opt_add(r.weight, ShortestAllVisitFrom(src_routes, r.dst,
                                                              visited)));
    visited->erase(r.dst);
  }
  VLOG(1) << prefix << "ShortestAllVisitFrom = "
          << (!min ? "nullopt" : absl::StrCat(*min));
  return min;
}

absl::optional<int> ShortestAllVisit(
    const absl::flat_hash_map<absl::string_view, std::vector<Route>>&
        src_routes) {
  absl::optional<int> min;
  absl::flat_hash_set<absl::string_view> visited;
  for (const auto& pair : src_routes) {
    visited.insert(pair.first);
    min = opt_min(min, ShortestAllVisitFrom(src_routes, pair.first, &visited));
    visited.erase(pair.first);
  }
  return *min;
}

absl::optional<int> LongestAllVisitFrom(
    const absl::flat_hash_map<absl::string_view, std::vector<Route>>&
        src_routes,
    absl::string_view from, absl::flat_hash_set<absl::string_view>* visited) {
  std::string prefix;
  prefix.resize(visited->size() * 2 - 2, ' ');
  VLOG(1) << prefix << "LongestAllVisitFrom(" << from << ", " << visited->size()
          << ")";
  if (visited->size() == src_routes.size()) return 0;
  absl::optional<int> max;
  auto it = src_routes.find(from);
  if (it == src_routes.end()) return absl::nullopt;

  for (const Route& r : it->second) {
    if (visited->contains(r.dst)) continue;
    visited->insert(r.dst);
    max = opt_max(max, opt_add(r.weight, LongestAllVisitFrom(src_routes, r.dst,
                                                             visited)));
    visited->erase(r.dst);
  }
  VLOG(1) << prefix << "LongestAllVisitFrom = "
          << (!max ? "nullopt" : absl::StrCat(*max));
  return max;
}

absl::optional<int> LongestAllVisit(
    const absl::flat_hash_map<absl::string_view, std::vector<Route>>&
        src_routes) {
  absl::optional<int> max;
  absl::flat_hash_set<absl::string_view> visited;
  for (const auto& pair : src_routes) {
    visited.insert(pair.first);
    max = opt_max(max, LongestAllVisitFrom(src_routes, pair.first, &visited));
    visited.erase(pair.first);
  }
  return *max;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_09::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, std::vector<Route>> src_routes;
  for (absl::string_view str : input) {
    Route r;
    if (!RE2::FullMatch(str, "(.*) to (.*) = (\\d+)", &r.src, &r.dst,
                        &r.weight)) {
      return Error("Bad input");
    }
    src_routes[r.src].push_back(r);
    src_routes[r.dst].push_back({r.dst, r.src, r.weight});
  }
  return IntReturn(ShortestAllVisit(src_routes));
}

absl::StatusOr<std::string> Day_2015_09::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, std::vector<Route>> src_routes;
  for (absl::string_view str : input) {
    Route r;
    if (!RE2::FullMatch(str, "(.*) to (.*) = (\\d+)", &r.src, &r.dst,
                        &r.weight)) {
      return Error("Bad input");
    }
    src_routes[r.src].push_back(r);
    src_routes[r.dst].push_back({r.dst, r.src, r.weight});
  }
  return IntReturn(LongestAllVisit(src_routes));
}

}  // namespace advent_of_code
