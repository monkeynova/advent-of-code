// http://adventofcode.com/2023/day/2

#include "advent_of_code/2023/day02/day02.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Game {
 public:
  struct View {
    int red = 0;
    int green = 0;
    int blue = 0;
  };

  static absl::StatusOr<Game> Parse(std::string_view line);

  int num() const { return num_; }
  absl::Span<const View> views() const { return views_; }

 private:
  Game() = default;

  int num_;
  std::vector<View> views_;
};

absl::StatusOr<Game> Game::Parse(std::string_view line) {
  Game ret;
  auto [game_str, view_set] = PairSplit(line, ": ");
  static LazyRE2 game_re{"Game (\\d+)"};
  if (!RE2::FullMatch(game_str, *game_re, &ret.num_)) {
    return Error("Bad game: ", game_str, "; ", line);
  }
  for (std::string_view view_str : absl::StrSplit(view_set, "; ")) {
    Game::View view;
    Game::View dupe_check;
    for (std::string_view ball_res : absl::StrSplit(view_str, ", ")) {
      int count;
      std::string_view color;
      static LazyRE2 ball_re{"(\\d+) (blue|red|green)"};
      if (!RE2::FullMatch(ball_res, *ball_re, &count, &color)) {
        return Error("Bad ball_res: ", ball_res, "; ", line);
      }
      if (color == "red") {
        if (dupe_check.red) return Error("Dupe red: ", line);
        dupe_check.red = 1;
        view.red = count;
      } else if (color == "blue") {
        if (dupe_check.blue) return Error("Dupe red: ", line);
        dupe_check.blue = 1;
        view.blue = count;
      } else if (color == "green") {
        if (dupe_check.green) return Error("Dupe red: ", line);
        dupe_check.green = 1;
        view.green = count;
      } else {
        return Error("Bad color: ", color, "; ", line);
      }
    }
    ret.views_.push_back(view);
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_02::Part1(
    absl::Span<std::string_view> input) const {
  int game_total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Game game, Game::Parse(line));
    auto is_possible = [](const Game::View& v) {
      return v.red <= 12 && v.green <= 13 && v.blue <= 14;
    };
    if (absl::c_all_of(game.views(), is_possible)) {
      game_total += game.num();
    }
  }
  return AdventReturn(game_total);
}

absl::StatusOr<std::string> Day_2023_02::Part2(
    absl::Span<std::string_view> input) const {
  int total_power = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Game game, Game::Parse(line));
    Game::View min = absl::c_accumulate(
      game.views(), Game::View{},
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
