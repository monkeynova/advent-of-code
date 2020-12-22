#include "advent_of_code/2020/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

void PlayWar(std::deque<int>* deck1, std::deque<int>* deck2) {
  while (!deck1->empty() && !deck2->empty()) {
    int c1 = deck1->front();
    int c2 = deck2->front();
    deck1->pop_front();
    deck2->pop_front();
    if (c1 > c2) {
      deck1->push_back(c1);
      deck1->push_back(c2);
    } else {
      deck2->push_back(c2);
      deck2->push_back(c1);
    }
  }
}

int ScoreDeck(const std::deque<int>& deck) {
  int score = 0;
  for (int i = 0; i < deck.size(); ++i) {
    score += (deck.size() - i) * deck[i];
  }
  return score;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day22_2020::Part1(
    absl::Span<absl::string_view> input) const {
  std::deque<int> deck1;
  std::deque<int> deck2;
  std::deque<int>* cur_deck = nullptr;
  for (absl::string_view str : input) {
    if (str.empty()) continue;
    int player;
    int card;
    if (RE2::FullMatch(str, "Player (\\d+):", &player)) {
      if (player == 1) cur_deck = &deck1;
      else if (player == 2) cur_deck = &deck2;
      else return Error("Bad player: ", player);
    } else if (absl::SimpleAtoi(str, &card)) {
      if (cur_deck == nullptr) return Error("No deck");
      cur_deck->push_back(card);
    } else {
      return Error("Bad input: ", str);
    }
  }
  PlayWar(&deck1, &deck2);
  if (deck1.empty()) {
    return IntReturn(ScoreDeck(deck2));
  }
  if (deck2.empty()) {
    return IntReturn(ScoreDeck(deck1));
  }
  return Error("Can't find empty deck");
}

absl::StatusOr<std::vector<std::string>> Day22_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
