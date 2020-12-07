#include "advent_of_code/2019/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

std::vector<int64_t> NewStack(const std::vector<int64_t>& deck) {
  std::vector<int64_t> out_deck(deck.size());
  for (int i = 0; i < deck.size(); ++i) {
    out_deck[deck.size() - i - 1] = deck[i];
  }
  return out_deck;
}

std::vector<int64_t> CutN(const std::vector<int64_t>& deck, int64_t n) {
  std::vector<int64_t> out_deck(deck.size());
  for (int i = 0; i < deck.size(); ++i) {
    out_deck[(deck.size() + i - n) % deck.size()] = deck[i];
  }
  return out_deck;
}

std::vector<int64_t> IncrementN(const std::vector<int64_t>& deck, int64_t n) {
  std::vector<int64_t> out_deck(deck.size());
  for (int i = 0; i < deck.size(); ++i) {
    out_deck[(i * n) % deck.size()] = deck[i];
  }
  return out_deck;
}

void Diagnostics() {
  std::vector<int64_t> deck;
  deck.reserve(10);
  for (int i = 0; i < 10; ++i) {
    deck.push_back(i);
  }
  LOG(WARNING) << absl::StrJoin(deck, ",");
  LOG(WARNING) << absl::StrJoin(NewStack(deck), ",");
  LOG(WARNING) << absl::StrJoin(IncrementN(deck, 3), ",");
  LOG(WARNING) << absl::StrJoin(CutN(deck, 3), ",");
  LOG(WARNING) << absl::StrJoin(CutN(deck, -3), ",");
}

absl::StatusOr<std::vector<std::string>> Day22_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  Diagnostics();
  std::vector<int64_t> deck;
  deck.reserve(10007);
  for (int i = 0; i < 10007; ++i) {
    deck.push_back(i);
  }
  for (absl::string_view str : input) {
    if (RE2::FullMatch(str, "deal into new stack")) {
      deck = NewStack(deck);
    } else if (int64_t inc = 0; RE2::FullMatch(str, "deal with increment (-?\\d+)", &inc)) {
      deck = IncrementN(deck, inc);
    } else if (int64_t cut = 0; RE2::FullMatch(str, "cut (-?\\d+)", &cut)) {
      deck = CutN(deck, cut);
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad command: ", str));
    }
  }
  for (int i = 0; i < deck.size(); ++i) {
    if (deck[i] == 2019) return IntReturn(i);
  }
  return absl::InvalidArgumentError("Can't find card 2019");
}

absl::StatusOr<std::vector<std::string>> Day22_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}
