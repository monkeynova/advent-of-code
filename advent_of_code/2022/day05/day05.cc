#include "advent_of_code/2022/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

using CargoStack = std::vector<char>;

absl::StatusOr<std::vector<std::unique_ptr<CargoStack>>> ParseStacks(
    absl::Span<std::string_view> lines) {
  std::vector<std::unique_ptr<CargoStack>> stacks;
  for (std::string_view line : lines) {
    if (line.size() % 4 != 3) return Error("Bad line");
    for (int i = 0; 4 * i < line.size(); ++i) {
      while (stacks.size() <= i) {
        stacks.push_back(absl::make_unique<CargoStack>());
      }
      std::string_view crate = line.substr(4 * i, 3);
      if (crate == "   ") {
        if (!stacks[i]->empty()) return Error("Hole in stack");
      } else if (crate[0] == '[' && crate[2] == ']') {
        stacks[i]->push_back(crate[1]);
      } else if (crate[0] == ' ' && crate[2] == ' ') {
        if (crate[1] != i + '1') return Error("Bad crate (index): ", crate);
      } else {
        return Error("Bad crate: ", crate);
      }
    }
  }

  // Stacks were added top to bottom. We want `back` to reflect the "top" so
  // reverse each of the stacks.
  for (std::unique_ptr<CargoStack>& s : stacks) {
    absl::c_reverse(*s);
  }
  return stacks;
}

struct Move {
  int count;
  CargoStack& from;
  CargoStack& to;
};

absl::StatusOr<std::vector<Move>> ParseMoves(
    absl::Span<std::string_view> moves,
    std::vector<std::unique_ptr<CargoStack>>& stacks) {
  std::vector<Move> ret;
  for (std::string_view line : moves) {
    int count, from_idx, to_idx;
    if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)", &count,
                        &from_idx, &to_idx)) {
      return Error("Bad move (RE)");
    }
    if (stacks.size() < from_idx) return Error("Bad move (from) ", from_idx);
    if (stacks.size() < to_idx) return Error("Bad move (to) ", to_idx);
    ret.push_back(Move{.count = count,
                       .from = *stacks[from_idx - 1],
                       .to = *stacks[to_idx - 1]});
  }
  return ret;
}

struct Day {
  std::vector<std::unique_ptr<CargoStack>> stacks;
  std::vector<Move> moves;

  std::string Tops() {
    std::string ret;
    for (const std::unique_ptr<CargoStack>& s : stacks) {
      ret.append(1, s->back());
    }
    return ret;
  }
};

absl::StatusOr<Day> Parse(absl::Span<std::string_view> input) {
  std::optional<int> split_at;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      split_at = i;
      break;
    }
  }
  if (!split_at) return Error("No empty line in input");

  ASSIGN_OR_RETURN(std::vector<std::unique_ptr<CargoStack>> stacks,
                   ParseStacks(input.subspan(0, *split_at)));

  ASSIGN_OR_RETURN(std::vector<Move> moves,
                   ParseMoves(input.subspan(*split_at + 1), stacks));

  return Day{.stacks = std::move(stacks), .moves = std::move(moves)};
}

}  // namespace

absl::StatusOr<std::string> Day_2022_05::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(Day day, Parse(input));

  for (const Move& m : day.moves) {
    if (m.from.size() < m.count) return Error("Bad move: empty");
    for (int i = 0; i < m.count; ++i) {
      m.to.push_back(m.from[m.from.size() - i - 1]);
    }
    m.from.resize(m.from.size() - m.count);
  }

  return day.Tops();
}

absl::StatusOr<std::string> Day_2022_05::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(Day day, Parse(input));

  for (const Move& m : day.moves) {
    if (m.from.size() < m.count) return Error("Bad move: empty");
    for (int i = 0; i < m.count; ++i) {
      m.to.push_back(m.from[m.from.size() - m.count + i]);
    }
    m.from.resize(m.from.size() - m.count);
  }

  return day.Tops();
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2022_05()); });

}  // namespace advent_of_code
