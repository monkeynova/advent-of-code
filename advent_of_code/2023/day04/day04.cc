// http://adventofcode.com/2023/day/4

#include "advent_of_code/2023/day04/day04.h"

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

std::string_view NextToken(std::string_view& line) {
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

struct Card {
  int num;
  absl::flat_hash_set<int> wins;
  std::vector<int> my_nums;

  int Score() const {
    return absl::c_accumulate(my_nums, 0, [&](int a, int n) {
      return a + (wins.contains(n) ? 1 : 0);
    });
  }
};

absl::StatusOr<Card> ParseCard(std::string_view input) {
  if (NextToken(input) != "Card") return Error("Bad card");
  Card card;
  if (!absl::SimpleAtoi(NextToken(input), &card.num)) return Error("Bad card");
  if (NextToken(input) != ":") return Error("Bad card");
  while (!input.empty()) {
    std::string_view token = NextToken(input);
    if (token == "|") break;
    int win;
    if (!absl::SimpleAtoi(token, &win)) return Error("Bad card");
    card.wins.insert(win);
  }
  while (!input.empty()) {
    int my_num;
    if (!absl::SimpleAtoi(NextToken(input), &my_num)) return Error("Bad card");
    card.my_nums.push_back(my_num);
  }
  return card;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_04::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Card card, ParseCard(line));
    int win_count = card.Score();
    if (win_count > 0) {
      total += 1 << (win_count - 1);
    }
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_04::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<int64_t> counts(input.size(), 1);

  int card_idx = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Card card, ParseCard(line));
    int win_count = card.Score();
    if (win_count > 0) {
      for (int i = 0; card_idx + 1 + i < counts.size() && i < win_count; ++i) {
        counts[card_idx + 1 + i] += counts[card_idx];
      }
    }
    ++card_idx;
  }
  return AdventReturn(absl::c_accumulate(counts, int64_t{0}));
}

}  // namespace advent_of_code
