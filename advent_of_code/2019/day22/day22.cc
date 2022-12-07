// https://adventofcode.com/2019/day/22
//
// --- Day 22: Slam Shuffle ---
// ----------------------------
//
// There isn't much to do while you wait for the droids to repair your
// ship. At least you're drifting in the right direction. You decide to
// practice a new card shuffle you've been working on.
//
// Digging through the ship's storage, you find a deck of space cards!
// Just like any deck of space cards, there are 10007 cards in the deck
// numbered 0 through 10006. The deck must be new - they're still in
// factory order, with 0 on the top, then 1, then 2, and so on, all the
// way through to 10006 on the bottom.
//
// You've been practicing three different techniques that you use while
// shuffling. Suppose you have a deck of only 10 cards (numbered 0
// through 9):
//
// To deal into new stack, create a new stack of cards by dealing the top
// card of the deck onto the top of the new stack repeatedly until you
// run out of cards:
//
// Top          Bottom
// 0 1 2 3 4 5 6 7 8 9   Your deck
// New stack
//
// 1 2 3 4 5 6 7 8 9   Your deck
// 0   New stack
//
// 2 3 4 5 6 7 8 9   Your deck
// 1 0   New stack
//
// 3 4 5 6 7 8 9   Your deck
// 2 1 0   New stack
//
// Several steps later...
//
// 9   Your deck
// 8 7 6 5 4 3 2 1 0   New stack
//
// Your deck
// 9 8 7 6 5 4 3 2 1 0   New stack
//
// Finally, pick up the new stack you've just created and use it as the
// deck for the next technique.
//
// To cut N cards, take the top N cards off the top of the deck and move
// them as a single unit to the bottom of the deck, retaining their
// order. For example, to cut 3:
//
// Top          Bottom
// 0 1 2 3 4 5 6 7 8 9   Your deck
//
// 3 4 5 6 7 8 9   Your deck
// 0 1 2                 Cut cards
//
// 3 4 5 6 7 8 9         Your deck
// 0 1 2   Cut cards
//
// 3 4 5 6 7 8 9 0 1 2   Your deck
//
// You've also been getting pretty good at a version of this technique
// where N is negative! In that case, cut (the absolute value of) N cards
// from the bottom of the deck onto the top. For example, to cut -4:
//
// Top          Bottom
// 0 1 2 3 4 5 6 7 8 9   Your deck
//
// 0 1 2 3 4 5           Your deck
// 6 7 8 9   Cut cards
//
// 0 1 2 3 4 5   Your deck
// 6 7 8 9               Cut cards
//
// 6 7 8 9 0 1 2 3 4 5   Your deck
//
// To deal with increment N, start by clearing enough space on your table
// to lay out all of the cards individually in a long line. Deal the top
// card into the leftmost position. Then, move N positions to the right
// and deal the next card there. If you would move into a position past
// the end of the space on your table, wrap around and keep counting from
// the leftmost card again. Continue this process until you run out of
// cards.
//
// For example, to deal with increment 3:
//
// 0 1 2 3 4 5 6 7 8 9   Your deck
// . . . . . . . . . .   Space on table
// ^                     Current position
//
// Deal the top card to the current position:
//
// 1 2 3 4 5 6 7 8 9   Your deck
// 0 . . . . . . . . .   Space on table
// ^                     Current position
//
// Move the current position right 3:
//
// 1 2 3 4 5 6 7 8 9   Your deck
// 0 . . . . . . . . .   Space on table
// ^               Current position
//
// Deal the top card:
//
// 2 3 4 5 6 7 8 9   Your deck
// 0 . . 1 . . . . . .   Space on table
// ^               Current position
//
// Move right 3 and deal:
//
// 3 4 5 6 7 8 9   Your deck
// 0 . . 1 . . 2 . . .   Space on table
// ^         Current position
//
// Move right 3 and deal:
//
// 4 5 6 7 8 9   Your deck
// 0 . . 1 . . 2 . . 3   Space on table
// ^   Current position
//
// Move right 3, wrapping around, and deal:
//
// 5 6 7 8 9   Your deck
// 0 . 4 1 . . 2 . . 3   Space on table
// ^                 Current position
//
// And so on:
//
// 0 7 4 1 8 5 2 9 6 3   Space on table
//
// Positions on the table which already contain cards are still counted;
// they're not skipped. Of course, this technique is carefully designed
// so it will never put two cards in the same position or leave a
// position empty.
//
// Finally, collect the cards on the table so that the leftmost card ends
// up at the top of your deck, the card to its right ends up just below
// the top card, and so on, until the rightmost card ends up at the
// bottom of the deck.
//
// The complete shuffle process (your puzzle input) consists of applying
// many of these techniques. Here are some examples that combine
// techniques; they all start with a factory order deck of 10 cards:
//
// deal with increment 7
// deal into new stack
// deal into new stack
// Result: 0 3 6 9 2 5 8 1 4 7
//
// cut 6
// deal with increment 7
// deal into new stack
// Result: 3 0 7 4 1 8 5 2 9 6
//
// deal with increment 7
// deal with increment 9
// cut -2
// Result: 6 3 0 7 4 1 8 5 2 9
//
// deal into new stack
// cut -2
// deal with increment 7
// cut 8
// cut -4
// deal with increment 7
// cut 3
// deal with increment 9
// deal with increment 3
// cut -1
// Result: 9 2 5 8 1 4 7 0 3 6
//
// Positions within the deck count from 0 at the top, then 1 for the card
// immediately below the top card, and so on to the bottom. (That is,
// cards start in the position matching their number.)
//
// After shuffling your factory order deck of 10007 cards, what is the
// position of card 2019?
//
// --- Part Two ---
// ----------------
//
// After a while, you realize your shuffling skill won't improve much
// more with merely a single deck of cards. You ask every 3D printer on
// the ship to make you some more cards while you check on the ship
// repairs. While reviewing the work the droids have finished so far, you
// think you see Halley's Comet fly past!
//
// When you get back, you discover that the 3D printers have combined
// their power to create for you a single, giant, brand new, factory
// order deck of 119315717514047 space cards.
//
// Finally, a deck of cards worthy of shuffling!
//
// You decide to apply your complete shuffle process (your puzzle input)
// to the deck 101741582076661 times in a row.
//
// You'll need to be careful, though - one wrong move with this many
// cards and you might overflow your entire ship!
//
// After shuffling your new, giant, factory order deck that many times,
// what number is on the card that ends up in position 2020?

#include "advent_of_code/2019/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/mod.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Transform {
  absl::uint128 mult;
  absl::uint128 add;
  absl::uint128 mod;

  Transform operator*(const Transform& o) const {
    return {.mult = (mult * o.mult) % mod,
            .add = (mult * o.add + add) % mod,
            .mod = mod};
  }

  int64_t Apply(int64_t card_position) const {
    absl::uint128 cp_i128 = card_position;
    return static_cast<int64_t>(
        absl::Uint128Low64((cp_i128 * mult + add) % mod));
  }
};

std::ostream& operator<<(std::ostream& out, const Transform t) {
  return out << "(" << t.mult << "*n + " << t.add << ")%" << t.mod;
}

Transform NewStack(int64_t deck_size) {
  return Transform{
      .mult = deck_size - 1, .add = deck_size - 1, .mod = deck_size};
}

Transform CutN(int64_t n, int64_t deck_size) {
  return Transform{.mult = 1, .add = deck_size - n, .mod = deck_size};
}

Transform IncrementN(int64_t n, int64_t deck_size) {
  return Transform{.mult = n, .add = 0, .mod = deck_size};
}

Transform NewStackInverse(int64_t deck_size) {
  return Transform{
      .mult = deck_size - 1, .add = deck_size - 1, .mod = deck_size};
}

Transform CutNInverse(int64_t n, int64_t deck_size) {
  return Transform{.mult = 1, .add = deck_size + n, .mod = deck_size};
}

Transform IncrementNInverse(int64_t n, int64_t deck_size) {
  return Transform{.mult = InverseMod<absl::uint128>(n, deck_size),
                   .add = 0,
                   .mod = deck_size};
}

absl::StatusOr<std::pair<Transform, Transform>> CreateTransform(
    absl::Span<absl::string_view> input, int64_t deck_size) {
  Transform t{.mult = 1, .add = 0, .mod = deck_size};
  Transform inv_t{.mult = 1, .add = 0, .mod = deck_size};
  for (absl::string_view str : input) {
    if (RE2::FullMatch(str, "deal into new stack")) {
      t = NewStack(deck_size) * t;
      inv_t = inv_t * NewStackInverse(deck_size);
    } else if (int64_t inc = 0;
               RE2::FullMatch(str, "deal with increment (-?\\d+)", &inc)) {
      if (std::gcd(inc, deck_size) != 1) {
        return absl::InvalidArgumentError(
            "Increment amount must be relatively prime to deck size");
      }
      t = IncrementN(inc, deck_size) * t;
      inv_t = inv_t * IncrementNInverse(inc, deck_size);
    } else if (int64_t cut = 0; RE2::FullMatch(str, "cut (-?\\d+)", &cut)) {
      t = CutN(cut, deck_size) * t;
      inv_t = inv_t * CutNInverse(cut, deck_size);
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad command: ", str));
    }
  }

  return std::make_pair(t, inv_t);
}

}  // namespace

absl::StatusOr<std::string> Day_2019_22::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::pair<Transform, Transform>> pair =
      CreateTransform(input, /*deck_size=*/10'007);
  if (!pair.ok()) return pair.status();

  auto [t, _] = *pair;
  return IntReturn(t.Apply(2019));
}

absl::StatusOr<std::string> Day_2019_22::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t run_count = 101'741'582'076'661;
  absl::StatusOr<std::pair<Transform, Transform>> pair =
      CreateTransform(input, /*deck_size=*/119'315'717'514'047);
  if (!pair.ok()) return pair.status();

  auto [_, inv_t] = *pair;
  Transform full_reverse_transform{.mult = 1, .add = 0, .mod = inv_t.mod};
  Transform power_reverse_transform = inv_t;
  for (int64_t bit = 1; bit <= run_count; bit <<= 1) {
    if (run_count & bit) {
      full_reverse_transform = power_reverse_transform * full_reverse_transform;
    }
    power_reverse_transform = power_reverse_transform * power_reverse_transform;
  }
  return IntReturn(full_reverse_transform.Apply(2020));
}
}  // namespace advent_of_code
