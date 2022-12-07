// https://adventofcode.com/2022/day/5
//
// --- Day 5: Supply Stacks ---
// ----------------------------
// 
// The expedition can depart as soon as the final supplies have been
// unloaded from the ships. Supplies are stored in stacks of marked
// crates, but because the needed supplies are buried under many other
// crates, the crates need to be rearranged.
// 
// The ship has a giant cargo crane capable of moving crates between
// stacks. To ensure none of the crates get crushed or fall over, the
// crane operator will rearrange them in a series of carefully-planned
// steps. After the crates are rearranged, the desired crates will be at
// the top of each stack.
// 
// The Elves don't want to interrupt the crane operator during this
// delicate procedure, but they forgot to ask her which crate will end up
// where, and they want to be ready to unload them as soon as possible so
// they can embark.
// 
// They do, however, have a drawing of the starting stacks of crates and
// the rearrangement procedure (your puzzle input). For example:
// 
// [D]    
// [N] [C]    
// [Z] [M] [P]
// 1   2   3 
// 
// move 1 from 2 to 1
// move 3 from 1 to 3
// move 2 from 2 to 1
// move 1 from 1 to 2
// 
// In this example, there are three stacks of crates. Stack 1 contains
// two crates: crate Z is on the bottom, and crate N is on top. Stack 2
// contains three crates; from bottom to top, they are crates M, C, and D.
// Finally, stack 3 contains a single crate, P.
// 
// Then, the rearrangement procedure is given. In each step of the
// procedure, a quantity of crates is moved from one stack to a different
// stack. In the first step of the above rearrangement procedure, one
// crate is moved from stack 2 to stack 1, resulting in this
// configuration:
// 
// [D]        
// [N] [C]    
// [Z] [M] [P]
// 1   2   3 
// 
// In the second step, three crates are moved from stack 1 to stack 3.
// Crates are moved one at a time, so the first crate to be moved (D)
// ends up below the second and third crates:
// 
// [Z]
// [N]
// [C] [D]
// [M] [P]
// 1   2   3
// 
// Then, both crates are moved from stack 2 to stack 1. Again, because
// crates are moved one at a time, crate C ends up below crate M:
// 
// [Z]
// [N]
// [M]     [D]
// [C]     [P]
// 1   2   3
// 
// Finally, one crate is moved from stack 1 to stack 2:
// 
// [   Z   ]
// [N]
// [D]
// [   C   ] [   M   ] [P]
// 1   2   3
// 
// The Elves just need to know which crate will end up on top of each
// stack; in this example, the top crates are C in stack 1, M in stack 2,
// and Z in stack 3, so you should combine these together and give the
// Elves the message CMZ.
// 
// After the rearrangement procedure completes, what crate ends up on top
// of each stack?
//
// --- Part Two ---
// ----------------
// 
// As you watch the crane operator expertly rearrange the crates, you
// notice the process isn't following your prediction.
// 
// Some mud was covering the writing on the side of the crane, and you
// quickly wipe it away. The crane isn't a CrateMover 9000 - it's a
// CrateMover 9001.
// 
// The CrateMover 9001 is notable for many new and exciting features: air
// conditioning, leather seats, an extra cup holder, and the ability to
// pick up and move multiple crates at once.
// 
// Again considering the example above, the crates begin in the same
// configuration:
// 
// [D]    
// [N] [C]    
// [Z] [M] [P]
// 1   2   3 
// 
// Moving a single crate from stack 2 to stack 1 behaves the same as
// before:
// 
// [D]        
// [N] [C]    
// [Z] [M] [P]
// 1   2   3 
// 
// However, the action of moving three crates from stack 1 to stack 3
// means that those three moved crates stay in the same order, resulting
// in this new configuration:
// 
// [D]
// [N]
// [C] [Z]
// [M] [P]
// 1   2   3
// 
// Next, as both crates are moved from stack 2 to stack 1, they retain
// their order as well:
// 
// [D]
// [N]
// [C]     [Z]
// [M]     [P]
// 1   2   3
// 
// Finally, a single crate is still moved from stack 1 to stack 2, but
// now it's crate C that gets moved:
// 
// [   D   ]
// [N]
// [Z]
// [   M   ] [   C   ] [P]
// 1   2   3
// 
// In this example, the CrateMover 9001 has put the crates in a totally
// different order: MCD.
// 
// Before the rearrangement process finishes, update your simulation so
// that the Elves know where they should stand to be ready to unload the
// final supplies. After the rearrangement procedure completes, what
// crate ends up on top of each stack?


#include "advent_of_code/2022/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

using CargoStack = std::vector<char>;

absl::StatusOr<std::vector<std::unique_ptr<CargoStack>>> ParseStacks(
    absl::Span<absl::string_view> lines) {
  std::vector<std::unique_ptr<CargoStack>> stacks;
  for (absl::string_view line : lines) {
    if (line.size() % 4 != 3) return Error("Bad line");
    for (int i = 0; 4 * i < line.size(); ++i) {
      while (stacks.size() <= i) {
        stacks.push_back(absl::make_unique<CargoStack>());
      }
      absl::string_view crate = line.substr(4 * i, 3);
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
    absl::Span<absl::string_view> moves,
    std::vector<std::unique_ptr<CargoStack>>& stacks) {
  std::vector<Move> ret;
  for (absl::string_view line : moves) {
    int count, from_idx, to_idx;
    if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)",
                        &count, &from_idx, &to_idx)) {
      return Error("Bad move (RE)");
    }
    if (stacks.size() < from_idx) return Error("Bad move (from) ", from_idx);
    if (stacks.size() < to_idx) return Error("Bad move (to) ", to_idx);
    ret.push_back(Move{.count = count, .from = *stacks[from_idx - 1],
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

absl::StatusOr<Day> Parse(absl::Span<absl::string_view> input) {
  std::optional<int> split_at;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      split_at = i;
      break;
    }
  }
  if (!split_at) return Error("No empty line in input");

  absl::StatusOr<std::vector<std::unique_ptr<CargoStack>>> stacks =
      ParseStacks(input.subspan(0, *split_at));
  if (!stacks.ok()) return stacks.status();

  absl::StatusOr<std::vector<Move>> moves =
      ParseMoves(input.subspan(*split_at + 1), *stacks);

  return Day{.stacks = std::move(*stacks), .moves = std::move(*moves)};
}

}  // namespace

absl::StatusOr<std::string> Day_2022_05::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Day> day = Parse(input);

  for (const Move& m : day->moves) {
    if (m.from.size() < m.count) return Error("Bad move: empty");
    for (int i = 0; i < m.count; ++i) {
      m.to.push_back(m.from[m.from.size() - i - 1]);
    }
    m.from.resize(m.from.size() - m.count);
  }

  return day->Tops();
}

absl::StatusOr<std::string> Day_2022_05::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Day> day = Parse(input);

  for (const Move& m : day->moves) {
    if (m.from.size() < m.count) return Error("Bad move: empty");
    for (int i = 0; i < m.count; ++i) {
      m.to.push_back(m.from[m.from.size() - m.count + i]);
    }
    m.from.resize(m.from.size() - m.count);
  }

  return day->Tops();
}

}  // namespace advent_of_code
