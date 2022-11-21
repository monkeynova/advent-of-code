// http://adventofcode.com/2019/day/06

#include "advent_of_code/2019/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
ParseOrbits(absl::Span<absl::string_view> input) {
  absl::flat_hash_map<std::string, std::vector<std::string>> orbits;
  RE2 orbit_pattern{"(...)\\)(...)"};
  for (absl::string_view rec : input) {
    absl::string_view from;
    absl::string_view to;
    if (!RE2::FullMatch(rec, orbit_pattern, &from, &to)) {
      return absl::InvalidArgumentError("Input not an orbit");
    }
    orbits[from].push_back(std::string(to));
  }
  if (orbits["COM"].empty()) {
    return absl::InvalidArgumentError("Base orbit not found");
  }
  return orbits;
}

int CountOrbits(
    const absl::flat_hash_map<std::string, std::vector<std::string>>& orbits,
    absl::string_view src, int depth) {
  // Current src contains `depth` orbits.
  int orbit_count = depth;
  auto it = orbits.find(src);
  if (it == orbits.end()) return orbit_count;

  for (const std::string& dst : it->second) {
    orbit_count += CountOrbits(orbits, dst, depth + 1);
  }

  return orbit_count;
}

struct TransferRet {
  int transfer_size;
  int from_depth;
  int to_depth;
};

TransferRet FindTransfer(
    const absl::flat_hash_map<std::string, std::vector<std::string>>& orbits,
    absl::string_view cur, absl::string_view from, absl::string_view to) {
  if (cur == from) {
    return {.transfer_size = -1, .from_depth = 0, .to_depth = -1};
  }
  if (cur == to) {
    return {.transfer_size = -1, .from_depth = -1, .to_depth = 0};
  }

  TransferRet ret{.transfer_size = -1, .from_depth = -1, .to_depth = -1};
  auto it = orbits.find(cur);
  if (it == orbits.end()) return ret;

  for (const std::string& dst : it->second) {
    TransferRet sub_transfer = FindTransfer(orbits, dst, from, to);
    if (sub_transfer.transfer_size != -1)
      return sub_transfer;
    else if (sub_transfer.from_depth != -1)
      ret.from_depth = sub_transfer.from_depth + 1;
    else if (sub_transfer.to_depth != -1)
      ret.to_depth = sub_transfer.to_depth + 1;
  }
  if (ret.from_depth != -1 && ret.to_depth != -1) {
    ret.transfer_size = ret.from_depth + ret.to_depth - 2;
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_06::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
      orbits = ParseOrbits(input);
  if (!orbits.ok()) return orbits.status();

  int count_orbits = CountOrbits(*orbits, "COM", 0);
  return IntReturn(count_orbits);
}

absl::StatusOr<std::string> Day_2019_06::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
      orbits = ParseOrbits(input);
  if (!orbits.ok()) return orbits.status();

  TransferRet transfer = FindTransfer(*orbits, "COM", "YOU", "SAN");
  return IntReturn(transfer.transfer_size);
}

}  // namespace advent_of_code
