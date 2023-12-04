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
  
  static std::string_view NextToken(std::string_view& line);

  int num_;
  std::vector<View> views_;
};

std::string_view Game::NextToken(std::string_view& line) {
  int start = 0;
  while (start < line.size() && isspace(line[start])) {
    ++start;
  }
  if (start == line.size()) {
    line = "";
    return "";
  }
  int end = start + 1;
  if (line[end] == '-' || isdigit(line[end])) {
    ++end;
    while (end < line.size() && isdigit(line[end])) {
      ++end;
    }
  } else if (isalpha(line[end])) {
    while (end < line.size() && isalpha(line[end])) {
      ++end;
    }
  }
  std::string_view ret = line.substr(start, end - start);
  line = line.substr(end);
  return ret;
}

absl::StatusOr<Game> Game::Parse(std::string_view line) {
  Game ret;

  // line = 'Game \d+: $view(; $view)*'
  // view = '$ball(, $ball)*'
  // ball = '\d+ (red|green|blue)'
  if (NextToken(line) != "Game") return Error("Bad game");
  if (!absl::SimpleAtoi(NextToken(line), &ret.num_)) {
    return Error("Not game_num");
  }
  if (NextToken(line) != ":") return Error("Bad game");

  while (!line.empty()) {
    ret.views_.push_back({});
    View& cur = ret.views_.back();
    while (!line.empty()) {
      int count = 0;
      if (!absl::SimpleAtoi(NextToken(line), &count)) {
        return Error("Not number");
      }
      std::string_view color = NextToken(line);
      if (color == "red") {
        cur.red = count;
      } else if (color == "blue") {
        cur.blue = count;
      } else if (color == "green") {
        cur.green = count;
      } else {
        return Error("Bad color: ", color);
      }
      if (line.empty()) break;
      std::string_view delim = NextToken(line);
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
