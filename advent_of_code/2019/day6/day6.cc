#include "advent_of_code/2019/day6/day6.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

int CountOrbits(const absl::flat_hash_map<std::string, std::vector<std::string>>& orbits,
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

absl::StatusOr<std::vector<std::string>> Day6_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::flat_hash_map<std::string, std::vector<std::string>> orbits;
  for (absl::string_view rec : input) {
    absl::string_view from;
    absl::string_view to;
    if (!RE2::FullMatch(rec, "(...)\\)(...)", &from, &to)) {
      return absl::InvalidArgumentError("Input not an orbit");
    }
    orbits[from].push_back(std::string(to));
  }
  if (orbits["COM"].empty()) {
    return absl::InvalidArgumentError("Base orbit not found");
  }
  int count_orbits = CountOrbits(orbits, "COM", 0);
  
  return std::vector<std::string>{absl::StrCat(count_orbits)};
}

absl::StatusOr<std::vector<std::string>> Day6_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
