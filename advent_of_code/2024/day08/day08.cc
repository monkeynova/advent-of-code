// http://adventofcode.com/2024/day/8

#include "advent_of_code/2024/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_08::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_map<char, std::vector<Point>> antenae;
  for (Point p : b.range()) {
    if (b[p] != '.') antenae[b[p]].push_back(p);
  }
  absl::flat_hash_set<Point> antinodes;
  for (const auto& [_, p_list] : antenae) {
    for (int i = 0; i < p_list.size(); ++i) {
      for (int j = i + 1; j < p_list.size(); ++j) {
        Point p1 = p_list[i];
        Point p2 = p_list[j];
        Point a1 = p1 + (p1 - p2);
        Point a2 = p2 + (p2 - p1);
        if (b.OnBoard(a1)) antinodes.insert(a1);
        if (b.OnBoard(a2)) antinodes.insert(a2);
      }
    }
  }
  return AdventReturn(antinodes.size());
}

absl::StatusOr<std::string> Day_2024_08::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  absl::flat_hash_map<char, std::vector<Point>> antenae;
  for (Point p : b.range()) {
    if (b[p] != '.') antenae[b[p]].push_back(p);
  }
  absl::flat_hash_set<Point> antinodes;
  for (const auto& [_, p_list] : antenae) {
    for (int i = 0; i < p_list.size(); ++i) {
      for (int j = i + 1; j < p_list.size(); ++j) {
        Point p1 = p_list[i];
        Point p2 = p_list[j];
        Point d = (p1 - p2).min_step();
        for (Point p3 = p1; b.OnBoard(p3); p3 += d) {
          antinodes.insert(p3);
        }
        for (Point p3 = p1; b.OnBoard(p3); p3 -= d) {
          antinodes.insert(p3);
        }
      }
    }
  }
  return AdventReturn(antinodes.size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/8,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_08()); });

}  // namespace advent_of_code
