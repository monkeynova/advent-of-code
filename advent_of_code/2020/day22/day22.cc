#include "advent_of_code/2020/day22/day22.h"

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

bool PlayWarRecursiveRec(
    std::deque<int>* deck1, std::deque<int>* deck2, int depth,
    absl::flat_hash_map<std::pair<std::deque<int>, std::deque<int>>, bool>*
        memo) {
  auto memo_key = std::make_pair(*deck1, *deck2);
  if (auto it = memo->find(memo_key); it != memo->end()) return it->second;

  VLOG_EVERY_N(1, 7777) << "PlayWarRecursive: " << depth;
  absl::flat_hash_set<std::pair<std::deque<int>, std::deque<int>>> hist;
  for (int round = 0; !deck1->empty() && !deck2->empty(); ++round) {
    if (VLOG_IS_ON(2)) {
      VLOG(2) << "Depth=" << depth << "; Round=" << round;
      VLOG(2) << "Deck1: " << absl::StrJoin(*deck1, ",");
      VLOG(2) << "Deck2: " << absl::StrJoin(*deck2, ",");
    }
    auto hist_key = std::make_pair(*deck1, *deck2);
    if (hist.contains(hist_key)) return true;
    hist.insert(hist_key);
    int c1 = deck1->front();
    int c2 = deck2->front();
    deck1->pop_front();
    deck2->pop_front();
    bool player1_wins = c1 > c2;

    if (deck1->size() >= c1 && deck2->size() >= c2) {
      std::deque<int> sub1(deck1->begin(), deck1->begin() + c1);
      std::deque<int> sub2(deck2->begin(), deck2->begin() + c2);
      player1_wins = PlayWarRecursiveRec(&sub1, &sub2, depth + 1, memo);
    }

    VLOG(2) << "Depth=" << depth << "; Round=" << round;
    VLOG(2) << "Winner: " << (player1_wins ? "1" : "2");

    if (player1_wins) {
      deck1->push_back(c1);
      deck1->push_back(c2);
    } else {
      deck2->push_back(c2);
      deck2->push_back(c1);
    }
  }
  memo->emplace(memo_key, deck2->empty());
  return deck2->empty();
}

bool PlayWarRecursive(std::deque<int>* deck1, std::deque<int>* deck2) {
  absl::flat_hash_map<std::pair<std::deque<int>, std::deque<int>>, bool> memo;
  return PlayWarRecursiveRec(deck1, deck2, 0, &memo);
}

int ScoreDeck(const std::deque<int>& deck) {
  int score = 0;
  for (int i = 0; i < deck.size(); ++i) {
    score += (deck.size() - i) * deck[i];
  }
  return score;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_22::Part1(
    absl::Span<absl::string_view> input) const {
  std::deque<int> deck1;
  std::deque<int> deck2;
  std::deque<int>* cur_deck = nullptr;
  for (absl::string_view str : input) {
    if (str.empty()) continue;
    int player;
    int card;
    if (RE2::FullMatch(str, "Player (\\d+):", &player)) {
      if (player == 1)
        cur_deck = &deck1;
      else if (player == 2)
        cur_deck = &deck2;
      else
        return Error("Bad player: ", player);
    } else if (absl::SimpleAtoi(str, &card)) {
      if (cur_deck == nullptr) return Error("No deck");
      cur_deck->push_back(card);
    } else {
      return Error("Bad input: ", str);
    }
  }
  PlayWar(&deck1, &deck2);
  if (deck1.empty()) {
    return AdventReturn(ScoreDeck(deck2));
  }
  if (deck2.empty()) {
    return AdventReturn(ScoreDeck(deck1));
  }
  return Error("Can't find empty deck");
}

absl::StatusOr<std::string> Day_2020_22::Part2(
    absl::Span<absl::string_view> input) const {
  std::deque<int> deck1;
  std::deque<int> deck2;
  std::deque<int>* cur_deck = nullptr;
  for (absl::string_view str : input) {
    if (str.empty()) continue;
    int player;
    int card;
    if (RE2::FullMatch(str, "Player (\\d+):", &player)) {
      if (player == 1)
        cur_deck = &deck1;
      else if (player == 2)
        cur_deck = &deck2;
      else
        return Error("Bad player: ", player);
    } else if (absl::SimpleAtoi(str, &card)) {
      if (cur_deck == nullptr) return Error("No deck");
      cur_deck->push_back(card);
    } else {
      return Error("Bad input: ", str);
    }
  }
  if (PlayWarRecursive(&deck1, &deck2)) {
    return AdventReturn(ScoreDeck(deck1));
  } else {
    return AdventReturn(ScoreDeck(deck2));
  }
}

}  // namespace advent_of_code
