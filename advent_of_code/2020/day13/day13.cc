#include "advent_of_code/2020/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/mod.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_13::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int start;
  if (!absl::SimpleAtoi(input[0], &start)) return Error("Bad input");
  int min_delta = std::numeric_limits<int>::max();
  int min_id = 1;
  for (std::string_view piece : absl::StrSplit(input[1], ",")) {
    if (piece == "x") continue;
    int id;
    if (!absl::SimpleAtoi(piece, &id)) return Error("Bad id:", id);
    int delta = id - (start % id);
    if (delta < min_delta) {
      min_delta = delta;
      min_id = id;
    }
  }
  if (min_id == -1) return Error("Can't find minimum delta");
  return AdventReturn(min_delta * min_id);
}

absl::StatusOr<std::string> Day_2020_13::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int start;
  if (!absl::SimpleAtoi(input[0], &start)) return Error("Bad input");

  std::vector<std::string_view> ids = absl::StrSplit(input[1], ",");
  int max_id = -1;
  struct Route {
    int64_t id;
    int64_t offset;
  };
  std::vector<Route> routes;
  for (int i = 0; i < ids.size(); ++i) {
    if (ids[i] == "x") continue;
    int id;
    if (!absl::SimpleAtoi(ids[i], &id)) return Error("Bad id:", id);

    routes.push_back({.id = id, .offset = i});
    if (id > max_id) {
      max_id = id;
    }
  }

  int64_t full_cycle_time = 1;
  for (Route r : routes) {
    full_cycle_time = full_cycle_time * r.id / std::gcd(r.id, full_cycle_time);
  }
  VLOG(1) << "full_cycle_time: " << full_cycle_time;

  std::vector<std::pair<int64_t, int64_t>> remainders;
  for (Route r : routes) {
    remainders.emplace_back((r.id - r.offset) % r.id, r.id);
  }
  return AdventReturn(ChineseRemainder(remainders));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_13()); });

}  // namespace advent_of_code
