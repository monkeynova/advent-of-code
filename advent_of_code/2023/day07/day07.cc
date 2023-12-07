// http://adventofcode.com/2023/day/7

#include "advent_of_code/2023/day07/day07.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"

namespace advent_of_code {

namespace {

class Hand {
 public:
  static absl::StatusOr<Hand> Parse(std::string_view in);

  int64_t bid() const { return bid_; }

  void RescoreJokers();

  bool operator<(const Hand& o) const {
    if (pairs_ != o.pairs_) return pairs_ < o.pairs_;
    return cards_ < o.cards_;
  }

 private:
  std::array<int, 5> cards_;
  int64_t bid_;
  int64_t pairs_;
};

absl::StatusOr<Hand> Hand::Parse(std::string_view in) {
  static const std::array<int, 128> kCharMap = []() {
    std::array<int, 128> ret;
    for (int i = 0; i < ret.size(); ++i) ret[i] = -1;
    for (int i = '2'; i <= '9'; ++i) ret[i] = i - '0';
    ret['T'] = 10;
    ret['J'] = 11;
    ret['Q'] = 12;
    ret['K'] = 13;
    ret['A'] = 14;
    return ret;
  }();

  Hand ret;
  std::string_view deck = in.substr(0, 5);
  if (deck.size() != 5) return Error("Bad cards");
  for (int i = 0; i < 5; ++i) {
    ret.cards_[i] = kCharMap[deck[i]];
    if (ret.cards_[i] == -1) return Error("Bad card");
  }
  if (!absl::SimpleAtoi(in.substr(6), &ret.bid_)) {
    return Error("Bad bid");
  }
  ret.pairs_ = 0;
  for (int i = 0; i < 5; ++i) {
    for (int j = i + 1; j < 5; ++j) {
      if (ret.cards_[i] == ret.cards_[j]) ++ret.pairs_;
    }
  }
  return ret;
}

void Hand::RescoreJokers() {
  bool has_joker = false;
  for (int i = 0; i < cards_.size(); ++i) {
    if (cards_[i] == 11) {
      cards_[i] = 1;
      has_joker = true;
    }
  }
  if (!has_joker) return;

  pairs_ = 0;
  for (int j_replace = 2; j_replace <= 14; ++j_replace) {
    int this_pairs = 0;
    std::array<int, 5> test_cards = cards_;
    for (int& c : test_cards) {
      if (c == 1) c = j_replace;
    }
    for (int i = 0; i < 5; ++i) {
      for (int j = i + 1; j < 5; ++j) {
        if (test_cards[i] == test_cards[j]) ++this_pairs;
     }
    }
    pairs_ = std::max<int64_t>(this_pairs, pairs_);
  }
}

}  // namespace

absl::StatusOr<std::string> Day_2023_07::Part1(
    absl::Span<std::string_view> input) const {
  std::vector<Hand> hands;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Hand h, Hand::Parse(line));
    hands.push_back(h);
  }

  absl::c_sort(hands);
  int64_t total = 0;
  for (int64_t i = 0; i < hands.size(); ++i) {
    total += (i + 1) * hands[i].bid();
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_07::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<Hand> hands;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Hand h, Hand::Parse(line));
    h.RescoreJokers();
    hands.push_back(h);
  }

  absl::c_sort(hands);
  int64_t total = 0;
  for (int64_t i = 0; i < hands.size(); ++i) {
    total += (i + 1) * hands[i].bid();
  }
  return AdventReturn(total);
}

}  // namespace advent_of_code
