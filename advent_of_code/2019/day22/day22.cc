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

int64_t NewStackTrackCard(int64_t card_position, int64_t deck_size) {
  return deck_size - card_position - 1;
}

int64_t CutNTrackCard(int64_t card_position, int64_t deck_size, int64_t n) {
  return (deck_size + card_position - n) % deck_size;
}

int64_t IncrementNTrackCard(int64_t card_position, int64_t deck_size, int64_t n) {
  return (card_position * n) % deck_size;
}

struct Transform {
  absl::uint128 mult;
  absl::uint128 add;
  absl::uint128 mod;

  int64_t Apply(int64_t card_position) const {
    absl::uint128 cp_i128 = card_position;
    return static_cast<int64_t>(absl::Uint128Low64((cp_i128 * mult + add) % mod));
  }
};

std::ostream& operator<<(std::ostream& out, const Transform t) {
  return out << "(" << t.mult << "*n + " << t.add << ")%" << t.mod;
}

Transform Combine(const Transform& a, const Transform& b) {
  return {.mult = (a.mult * b.mult) % a.mod, .add = (a.mult * b.add + a.add) % a.mod, .mod = a.mod};
}

Transform NewStack(Transform t) {
  return Combine(Transform{.mult = t.mod - 1, .add = t.mod - 1, .mod = t.mod}, t);
  return Transform{.mult = ((t.mod - 1) * t.mult) % t.mod, .add = (t.mod - t.add - 1) % t.mod, .mod = t.mod};
}

Transform CutN(Transform t, int64_t n) {
  return Combine(Transform{.mult = 1, .add = t.mod - n, .mod = t.mod}, t);
  return Transform{.mult = t.mult, .add = (t.mod + t.add - n) % t.mod, .mod = t.mod};
}

Transform IncrementN(Transform t, int64_t n) {
  return Combine(Transform{.mult = n, .add = 0, .mod = t.mod}, t);
  return Transform{.mult = (n * t.mult) % t.mod, .add = (n * t.add) % t.mod, .mod = t.mod};
}

Transform NewStackInverse(Transform t) {
  return Combine(t, Transform{.mult = t.mod - 1, .add = t.mod - 1, .mod = t.mod});
  return Transform{.mult = ((t.mod - 1) * t.mult) % t.mod, .add = (t.mod - t.add - 1) % t.mod, .mod = t.mod};
}

Transform CutNInverse(Transform t, int64_t n) {
  return Combine(t, Transform{.mult = 1, .add = t.mod + n, .mod = t.mod});
  return Transform{.mult = t.mult, .add = (t.mod + t.add + n) % t.mod, .mod = t.mod};
}

absl::uint128 PowerMod(absl::uint128 base, absl::uint128 exp, absl::uint128 mod) {
  absl::uint128 product = 1;
  absl::uint128 power_mult = base;
  for (int64_t bit = 1; bit <= exp; bit <<= 1) {
    if (exp & bit) product = (product * power_mult) % mod;
    power_mult = (power_mult * power_mult) % mod;
  }
  return product;
}

Transform IncrementNInverse(Transform t, int64_t n) {
  // TODO(@monkeynova): Error handling.
  CHECK(std::gcd(n, static_cast<int64_t>(absl::Uint128Low64(t.mod))) == 1);
  // If 'n' and 'mod' are relatively prime, n ** (mod - 1) == 1, which means
  // n ** -1 == n ** (mod - 2).
  absl::uint128 inverse_n = PowerMod(n, t.mod - 2, t.mod);
  return Combine(t, Transform{.mult = inverse_n, .add = 0, .mod = t.mod});
  return Transform{.mult = (inverse_n * t.mult) % t.mod, .add = (inverse_n * t.add) % t.mod, .mod = t.mod};
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

  Transform t{.mult = 1, .add = 0, .mod = 10007};
  LOG(WARNING) << Combine(CutNInverse(t, 4913), CutN(t, 4913));
  LOG(WARNING) << Combine(CutNInverse(t, -1034), CutN(t, -1034));
  LOG(WARNING) << Combine(IncrementNInverse(t, 49), IncrementN(t, 49));
  LOG(WARNING) << Combine(NewStackInverse(t), NewStack(t));

  Transform big_t{.mult = 1, .add = 0, .mod = 119315717514047};
  LOG(WARNING) << Combine(IncrementNInverse(big_t, 49), IncrementN(big_t, 49));
}

absl::StatusOr<std::vector<std::string>> Day22_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  Diagnostics();
  Transform t{.mult = 1, .add = 0, .mod = 10007};
  Transform inv_t{.mult = 1, .add = 0, .mod = 10007};
  for (absl::string_view str : input) {
    if (RE2::FullMatch(str, "deal into new stack")) {
      t = NewStack(t);
      inv_t = NewStackInverse(inv_t);
    } else if (int64_t inc = 0; RE2::FullMatch(str, "deal with increment (-?\\d+)", &inc)) {
      t = IncrementN(t, inc);
      inv_t = IncrementNInverse(inv_t, inc);
    } else if (int64_t cut = 0; RE2::FullMatch(str, "cut (-?\\d+)", &cut)) {
      t = CutN(t, cut);
      inv_t = CutNInverse(inv_t, cut);
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad command: ", str));
    }
  }
  LOG(WARNING) << "2019^2: " << t.Apply(t.Apply(2019));
  LOG(WARNING) << "t^2: " << Combine(t, t).Apply(2019);
  LOG(WARNING) << "2019^3: " << t.Apply(t.Apply(t.Apply(2019)));
  LOG(WARNING) << "t^2 * t: " << Combine(Combine(t, t), t).Apply(2019);
  LOG(WARNING) << "t * t^2: " << Combine(t, Combine(t, t)).Apply(2019);
  LOG(WARNING) << "t^-1 * t: " << inv_t.Apply(t.Apply(2019));
  return IntReturn(t.Apply(2019));
}

absl::StatusOr<std::vector<std::string>> Day22_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  int64_t deck_size = 119315717514047;
  int64_t run_count = 101741582076661;
  Transform t{.mult = 1, .add = 0, .mod = deck_size};
  Transform inv_t{.mult = 1, .add = 0, .mod = deck_size};
  for (absl::string_view str : input) {
    if (RE2::FullMatch(str, "deal into new stack")) {
      t = NewStack(t);
      inv_t = NewStackInverse(inv_t);
    } else if (int64_t inc = 0; RE2::FullMatch(str, "deal with increment (-?\\d+)", &inc)) {
      t = IncrementN(t, inc);
      inv_t = IncrementNInverse(inv_t, inc);
    } else if (int64_t cut = 0; RE2::FullMatch(str, "cut (-?\\d+)", &cut)) {
      t = CutN(t, cut);
      inv_t = CutNInverse(inv_t, cut);
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad command: ", str));
    }
  }
  LOG(WARNING) << "2019^2: " << t.Apply(t.Apply(2020));
  LOG(WARNING) << "t^2: " << Combine(t, t).Apply(2020);
  LOG(WARNING) << "2019^3: " << t.Apply(t.Apply(t.Apply(2020)));
  LOG(WARNING) << "t^2 * t: " << Combine(Combine(t, t), t).Apply(2020);
  LOG(WARNING) << "t * t^2: " << Combine(t, Combine(t, t)).Apply(2020);
  LOG(WARNING) << "t^-1 * t: " << inv_t.Apply(t.Apply(2020));
  Transform full_transform{.mult = 1, .add = 0, .mod = deck_size};
  Transform full_reverse_transform{.mult = 1, .add = 0, .mod = deck_size};
  Transform power_transform = t;
  Transform power_reverse_transform = inv_t;
  for (int64_t bit = 1; bit <= run_count; bit <<= 1) {
    if (run_count & bit) {
      full_transform = Combine(power_transform, full_transform);
      full_reverse_transform = Combine(power_reverse_transform, full_reverse_transform);
    }
    power_transform = Combine(power_transform, power_transform);
    power_reverse_transform = Combine(power_reverse_transform, power_reverse_transform);
  }
  LOG(WARNING) << "t(2020): " << full_transform.Apply(2020);
  LOG(WARNING) << "t^1(2020): " << full_reverse_transform.Apply(2020);
  LOG(WARNING) << "t^-1*t(2020): " << Combine(full_reverse_transform, full_transform).Apply(2020);
  LOG(WARNING) << "t*t^-1(2020): " << Combine(full_transform, full_reverse_transform).Apply(2020);
  return IntReturn(full_reverse_transform.Apply(2020));
}