#include "advent_of_code/2020/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2020_13::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int start;
  if (!absl::SimpleAtoi(input[0], &start)) return Error("Bad input");
  int min_delta = std::numeric_limits<int>::max();
  int min_id;
  for (absl::string_view piece : absl::StrSplit(input[1], ",")) {
    if (piece == "x") continue;
    int id;
    if (!absl::SimpleAtoi(piece, &id)) return Error("Bad id:", id);
    int delta = id - (start % id);
    if (delta < min_delta) {
      min_delta = delta;
      min_id = id;
    }
  }
  return IntReturn(min_delta * min_id);
}

absl::StatusOr<std::vector<std::string>> Day_2020_13::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int start;
  if (!absl::SimpleAtoi(input[0], &start)) return Error("Bad input");

  std::vector<absl::string_view> ids = absl::StrSplit(input[1], ",");
  int max_id = -1;
  int max_offset = 0;
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
      max_offset = i;
    }
  }

  int64_t full_cycle_time = 1;
  for (Route r : routes) {
    full_cycle_time = full_cycle_time * r.id / std::gcd(r.id, full_cycle_time);
  }
  VLOG(1) << "full_cycle_time: " << full_cycle_time;

  int64_t ts = 0;
  int64_t product = 1;
  for (Route r : routes) {
    while (true) {
      int64_t mod = ts % r.id;
      int64_t should_mod = (r.id - r.offset) % r.id;
      if (should_mod < 0) should_mod += r.id;
      if (mod == should_mod) break;
      ts += product;
      VLOG(2) << "ts: " << ts;
    }
    product = product * r.id / std::gcd(product, r.id);
    VLOG(1) << "prodict: " << product;
  }
  return IntReturn(ts);
}

}  // namespace advent_of_code
