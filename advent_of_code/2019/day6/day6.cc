#include "advent_of_code/2019/day6/day6.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
ParseOrbits(const std::vector<absl::string_view>& input) {
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
    return {.from_depth = 0, .transfer_size = -1, .to_depth = -1};
  }
  if (cur == to) {
    return {.to_depth = 0, .transfer_size = -1, .from_depth = -1};
  }

  TransferRet ret{.from_depth = -1, .transfer_size = -1, .to_depth = -1};
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

absl::StatusOr<std::vector<std::string>> Day6_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
      orbits = ParseOrbits(input);
  if (!orbits.ok()) return orbits.status();

  int count_orbits = CountOrbits(*orbits, "COM", 0);
  return std::vector<std::string>{absl::StrCat(count_orbits)};
}

absl::StatusOr<std::vector<std::string>> Day6_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
      orbits = ParseOrbits(input);
  if (!orbits.ok()) return orbits.status();

  TransferRet transfer = FindTransfer(*orbits, "COM", "YOU", "SAN");
  return std::vector<std::string>{absl::StrCat(transfer.transfer_size)};
}
