// http://adventofcode.com/2024/day/25

#include "advent_of_code/2024/day25/day25.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_25::Part1(
    absl::Span<std::string_view> input) const {
  std::vector<std::vector<int>> keys;
  std::vector<std::vector<int>> locks;

  if (input.size() % 8 != 7) return absl::InvalidArgumentError("size");
  for (int i = 0; i < input.size(); i += 8) {
    ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input.subspan(i, 7)));
    if (b.width() != 5) return absl::InvalidArgumentError("width");

    std::vector<int> rec(b.width(), -1);
    for (int x = 0; x < b.width(); ++x) {
      for (int y = 0; y < b.height(); ++y) {
        if (b[{x, y}] == '#') ++rec[x];
      }
    }
    if (input[i] == "#####") locks.push_back(rec);
    else if (input[i] == ".....") keys.push_back(rec);
    else return absl::InvalidArgumentError("key/lock");
  }

  int fits = 0;
  for (const auto& key : keys) {
    for (const auto& lock : locks) {
      bool match = true;
      for (int i = 0; i < 5; ++i) {
        if (key[i] + lock[i] >= 6) match = false;
      }
      if (match) ++fits;
    }
  }
  return AdventReturn(fits);
}

absl::StatusOr<std::string> Day_2024_25::Part2(
    absl::Span<std::string_view> input) const {
  return AdventReturn("Merry Christmas!");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/25,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_25()); });

}  // namespace advent_of_code
