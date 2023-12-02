// http://adventofcode.com/2023/day/2

#include "advent_of_code/2023/day02/day02.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2023_02::Part1(
    absl::Span<std::string_view> input) const {
  int game_total = 0;
  for (std::string_view line : input) {
    auto [game_str, view_set] = PairSplit(line, ": ");
    int game_num;
    if (!RE2::FullMatch(game_str, "Game (\\d+)", &game_num)) {
      return Error("Bad game: ", game_str);
    }
    bool bad = false;
    for (std::string_view view : absl::StrSplit(view_set, "; ")) {
      absl::flat_hash_map<std::string_view, int> totals;
      for (std::string_view ball_res : absl::StrSplit(view, ", ")) {
        int count;
        std::string_view color;
        if (!RE2::FullMatch(ball_res, "(\\d+) (blue|red|green)", &count, &color)) {
          return Error("Bad ball_res: ", ball_res);
        }
        if (totals.contains(color)) return Error("Dupe color in view:", color);
        totals[color] += count;
      }
      bad |= totals["blue"] > 14;
      bad |= totals["green"] > 13;
      bad |= totals["red"] > 12;
    }
    if (!bad) {
      game_total += game_num;
    }
  }
  return AdventReturn(game_total);
}

absl::StatusOr<std::string> Day_2023_02::Part2(
    absl::Span<std::string_view> input) const {
  int total_power = 0;
  for (std::string_view line : input) {
    auto [game_str, view_set] = PairSplit(line, ": ");
    int game_num;
    if (!RE2::FullMatch(game_str, "Game (\\d+)", &game_num)) {
      return Error("Bad game: ", game_str);
    }
    absl::flat_hash_map<std::string_view, int> totals;
    for (std::string_view view : absl::StrSplit(view_set, "; ")) {
      for (std::string_view ball_res : absl::StrSplit(view, ", ")) {
        int count;
        std::string_view color;
        if (!RE2::FullMatch(ball_res, "(\\d+) (blue|red|green)", &count, &color)) {
          return Error("Bad ball_res: ", ball_res);
        }
        totals[color] = std::max(totals[color], count);
      }
    }
    total_power += totals["red"] * totals["blue"] * totals["green"];
  }
  return AdventReturn(total_power);
}

}  // namespace advent_of_code
