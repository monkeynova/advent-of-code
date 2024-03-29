#include "advent_of_code/2019/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

absl::StatusOr<absl::flat_hash_map<std::string, std::vector<std::string>>>
ParseOrbits(absl::Span<std::string_view> input) {
  absl::flat_hash_map<std::string, std::vector<std::string>> orbits;
  RE2 orbit_pattern{"(...)\\)(...)"};
  for (std::string_view rec : input) {
    std::string_view from;
    std::string_view to;
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
    std::string_view src, int depth) {
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
    std::string_view cur, std::string_view from, std::string_view to) {
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
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string, std::vector<std::string>> orbits;
  ASSIGN_OR_RETURN(orbits, ParseOrbits(input));

  int count_orbits = CountOrbits(orbits, "COM", 0);
  return AdventReturn(count_orbits);
}

absl::StatusOr<std::string> Day_2019_06::Part2(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string, std::vector<std::string>> orbits;
  ASSIGN_OR_RETURN(orbits, ParseOrbits(input));

  TransferRet transfer = FindTransfer(orbits, "COM", "YOU", "SAN");
  return AdventReturn(transfer.transfer_size);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/6,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_06()); });

}  // namespace advent_of_code
