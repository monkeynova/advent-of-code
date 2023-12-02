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

struct Game {
  struct View {
    int red = 0;
    int green = 0;
    int blue = 0;
  };

  int num;
  std::vector<View> views;
};

absl::StatusOr<Game> Parse(std::string_view line) {
  Game ret;
  auto [game_str, view_set] = PairSplit(line, ": ");
  static LazyRE2 game_re{"Game (\\d+)"};
  if (!RE2::FullMatch(game_str, *game_re, &ret.num)) {
    return Error("Bad game: ", game_str);
  }
  for (std::string_view view_str : absl::StrSplit(view_set, "; ")) {
    Game::View view;
    for (std::string_view ball_res : absl::StrSplit(view_str, ", ")) {
      int count;
      std::string_view color;
      static LazyRE2 ball_re{"(\\d+) (blue|red|green)"};
      if (!RE2::FullMatch(ball_res, *ball_re, &count, &color)) {
        return Error("Bad ball_res: ", ball_res);
      }
      if (color == "red") view.red = count;
      else if (color == "blue") view.blue = count;
      else if (color == "green") view.green = count;
      else return Error("Bad color: ", color);
    }
    ret.views.push_back(view);
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_02::Part1(
    absl::Span<std::string_view> input) const {
  int game_total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Game game, Parse(line));
    auto is_possible = [](const Game::View& v) {
      return v.red <= 12 && v.green <= 13 && v.blue <= 14;
    };
    if (absl::c_all_of(game.views, is_possible)) {
      game_total += game.num;
    }
  }
  return AdventReturn(game_total);
}

absl::StatusOr<std::string> Day_2023_02::Part2(
    absl::Span<std::string_view> input) const {
  int total_power = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Game game, Parse(line));
    Game::View min = absl::c_accumulate(
      game.views, Game::View{},
      [](Game::View a, Game::View b) {
        a.red = std::max(a.red, b.red);
        a.green = std::max(a.green, b.green);
        a.blue = std::max(a.blue, b.blue);
        return a;
      });
    total_power += min.red * min.blue * min.green;
  }
  return AdventReturn(total_power);
}

}  // namespace advent_of_code
