// http://adventofcode.com/2023/day/4

#include "advent_of_code/2023/day04/day04.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

class Card {
 public:
  static absl::StatusOr<Card> Parse(std::string_view input);

  int num() const { return num_; }
  int Score() const {
    return absl::c_accumulate(my_nums_, 0, [&](int a, int n) {
      return a + (wins_[n] ? 1 : 0);
    });
  }

 private:
  int num_;
  std::bitset<100> wins_;
  std::vector<int> my_nums_;
};

absl::StatusOr<Card> Card::Parse(std::string_view input) {
  Tokenizer tok(input);
  if (tok.Next() != "Card") return Error("Bad card");
  Card card;
  if (!absl::SimpleAtoi(tok.Next(), &card.num_)) return Error("Bad card");
  if (tok.Next() != ":") return Error("Bad card");
  while (!tok.Done()) {
    std::string_view token = tok.Next();
    if (token == "|") break;
    int win;
    if (!absl::SimpleAtoi(token, &win)) return Error("Bad card");
    if (win >= 100) return Error("Win too big");
    card.wins_[win] = true;
  }
  while (!tok.Done()) {
    int my_num;
    if (!absl::SimpleAtoi(tok.Next(), &my_num)) return Error("Bad card");
    card.my_nums_.push_back(my_num);
  }
  return card;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_04::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Card card, Card::Parse(line));
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
    ASSIGN_OR_RETURN(Card card, Card::Parse(line));
    if (card.num() != card_idx + 1) return Error("Bad card_num");
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
