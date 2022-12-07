// https://adventofcode.com/2019/day/6
//
// --- Day 6: Universal Orbit Map ---
// ----------------------------------
//
// You've landed at the Universal Orbit Map facility on Mercury. Because
// navigation in space often involves transferring between orbits, the
// orbit maps here are useful for finding efficient routes between, for
// example, you and Santa. You download a map of the local orbits (your
// puzzle input).
//
// Except for the universal Center of Mass (COM), every object in space
// is in orbit around exactly one other object. An orbit looks roughly
// like this:
//
// \
// \
// |
// |
// AAA--> o            o <--BBB
// |
// |
// /
// /
//
// In this diagram, the object BBB is in orbit around AAA. The path that
// BBB takes around AAA (drawn with lines) is only partly shown. In the
// map data, this orbital relationship is written AAA)BBB, which means "BBB
// is in orbit around AAA".
//
// Before you use your map data to plot a course, you need to make sure
// it wasn't corrupted during the download. To verify maps, the Universal
// Orbit Map facility uses orbit count checksums - the total number of
// direct orbits (like the one shown above) and indirect orbits.
//
// Whenever A orbits B and B orbits C, then A indirectly orbits C. This
// chain can be any number of objects long: if A orbits B, B orbits C,
// and C orbits D, then A indirectly orbits D.
//
// For example, suppose you have the following map:
//
// COM)B
// B)C
// C)D
// D)E
// E)F
// B)G
// G)H
// D)I
// E)J
// J)K
// K)L
//
// Visually, the above map of orbits looks like this:
//
// G - H       J - K - L
// /           /
// COM - B - C - D - E - F
// \
// I
//
// In this visual representation, when two objects are connected by a
// line, the one on the right directly orbits the one on the left.
//
// Here, we can count the total number of orbits as follows:
//
// * D directly orbits C and indirectly orbits B and COM, a total of 3
// orbits.
//
// * L directly orbits K and indirectly orbits J, E, D, C, B, and COM,
// a total of 7 orbits.
//
// * COM orbits nothing.
//
// The total number of direct and indirect orbits in this example is 42.
//
// What is the total number of direct and indirect orbits in your map
// data?
//
// --- Part Two ---
// ----------------
//
// Now, you just need to figure out how many orbital transfers you (YOU)
// need to take to get to Santa (SAN).
//
// You start at the object YOU are orbiting; your destination is the
// object SAN is orbiting. An orbital transfer lets you move from any
// object to an object orbiting or orbited by that object.
//
// For example, suppose you have the following map:
//
// COM)B
// B)C
// C)D
// D)E
// E)F
// B)G
// G)H
// D)I
// E)J
// J)K
// K)L
// K)YOU
// I)SAN
//
// Visually, the above map of orbits looks like this:
//
// YOU                               /           G - H          J - K    - L
// /              /   COM - B - C -    D - E    - F
// \                      I - SAN
//
// In this example, YOU are in orbit around K, and SAN is in orbit around
// I. To move from K to I, a minimum of 4 orbital transfers are required:
//
// * K to J
//
// * J to E
//
// * E to D
//
// * D to I
//
// Afterward, the map of orbits looks like this:
//
// G - H       J - K - L
// /           /
// COM - B - C - D - E - F
// \
// I - SAN
// \                        YOU
//
// What is the minimum number of orbital transfers required to move from
// the object YOU are orbiting to the object SAN is orbiting? (Between
// the objects they are orbiting - not between YOU and SAN.)

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
