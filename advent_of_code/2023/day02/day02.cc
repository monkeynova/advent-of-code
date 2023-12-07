// http://adventofcode.com/2023/day/2

#include "advent_of_code/2023/day02/day02.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

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
  
  static std::string_view NextToken(std::string_view& line);

  int num_;
  std::vector<View> views_;
};

absl::StatusOr<Game> Game::Parse(std::string_view line) {
  Game ret;

  // line = 'Game \d+: $view(; $view)*'
  // view = '$ball(, $ball)*'
  // ball = '\d+ (red|green|blue)'
  Tokenizer tok(line);
  RETURN_IF_ERROR(tok.NextIs("Game"));
  ASSIGN_OR_RETURN(ret.num_, tok.NextInt());
  RETURN_IF_ERROR(tok.NextIs(":"));

  while (!tok.Done()) {
    ret.views_.push_back({});
    View& cur = ret.views_.back();
    while (!tok.Done()) {
      ASSIGN_OR_RETURN(int count, tok.NextInt());
      std::string_view color = tok.Next();
      if (color == "red") {
        cur.red = count;
      } else if (color == "blue") {
        cur.blue = count;
      } else if (color == "green") {
        cur.green = count;
      } else {
        return Error("Bad color: ", color);
      }
      if (tok.Done()) break;
      std::string_view delim = tok.Next();
      if (delim == ",") continue;
      if (delim == ";") break;
      return Error("Bad game");
    }
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
