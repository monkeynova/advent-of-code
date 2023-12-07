// http://adventofcode.com/2023/day/7

#include "advent_of_code/2023/day07/day07.h"

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
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Hand {
  std::string_view deck;
  std::array<int, 5> cards;
  int64_t bid;
  int64_t pairs;

  static absl::StatusOr<Hand> Parse(std::string_view in) {
    static const absl::flat_hash_map<char, int> kCharMap = {
      {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
      {'8', 8}, {'9', 9}, {'T', 10}, {'J', 11}, {'Q', 12}, {'K', 13},
      {'A', 14},
    };
    Hand ret;
    ret.deck = in.substr(0, 5);
    if (ret.deck.size() != 5) return Error("Bad cards");
    for (int i = 0; i < 5; ++i) {
      auto val_it = kCharMap.find(ret.deck[i]);
      if (val_it == kCharMap.end()) return Error("Bad card: ", ret.deck.substr(i, 1), " in ", in);
      ret.cards[i] = val_it->second;
    }
    if (!absl::SimpleAtoi(in.substr(6), &ret.bid)) {
      return Error("Bad bid");
    }
    ret.pairs = 0;
    for (int i = 0; i < 5; ++i) {
      for (int j = i + 1; j < 5; ++j) {
        if (ret.cards[i] == ret.cards[j]) ++ret.pairs;
      }
    }
    return ret;
  }

  bool operator<(const Hand& o) const {
    if (pairs != o.pairs) return pairs < o.pairs;
    return cards < o.cards;
  }
};

struct Hand2 {
  std::string_view deck;
  std::array<int, 5> cards;
  int64_t bid;
  int64_t pairs;

  static absl::StatusOr<Hand2> Parse(std::string_view in) {
    static const absl::flat_hash_map<char, int> kCharMap = {
      {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
      {'8', 8}, {'9', 9}, {'T', 10}, {'J', 1}, {'Q', 12}, {'K', 13},
      {'A', 14},
    };
    Hand2 ret;
    ret.deck = in.substr(0, 5);
    if (ret.deck.size() != 5) return Error("Bad cards");
    for (int i = 0; i < 5; ++i) {
      auto val_it = kCharMap.find(ret.deck[i]);
      if (val_it == kCharMap.end()) return Error("Bad card: ", ret.deck.substr(i, 1), " in ", in);
      ret.cards[i] = val_it->second;
    }
    if (!absl::SimpleAtoi(in.substr(6), &ret.bid)) {
      return Error("Bad bid");
    }
    ret.pairs = 0;
    for (const auto& [c, j_replace] : kCharMap) {
      if (c == 'J') continue;
      int this_pairs = 0;
      std::array<int, 5> test_cards = ret.cards;
      for (int& c : test_cards) {
        if (c == 1) c = j_replace;
      }

      for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
          if (test_cards[i] == test_cards[j]) ++this_pairs;
       }
      }
      ret.pairs = std::max<int64_t>(this_pairs, ret.pairs);
    }
    return ret;
  }

  bool operator<(const Hand2& o) const {
    if (pairs != o.pairs) return pairs < o.pairs;
    return cards < o.cards;
  }
};


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
    VLOG(2) << hands[i].deck << ": " << (i + 1) << " * " << hands[i].bid << "(" << hands[i].pairs << ")";
    total += (i + 1) * hands[i].bid;
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_07::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<Hand2> hands;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Hand2 h, Hand2::Parse(line));
    hands.push_back(h);
  }

  absl::c_sort(hands);
  int64_t total = 0;
  for (int64_t i = 0; i < hands.size(); ++i) {
    VLOG(2) << hands[i].deck << ": " << (i + 1) << " * " << hands[i].bid << "(" << hands[i].pairs << ")";
    total += (i + 1) * hands[i].bid;
  }
  return AdventReturn(total);
}

}  // namespace advent_of_code
