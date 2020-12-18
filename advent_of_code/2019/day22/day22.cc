#include "advent_of_code/2019/day22/day22.h"

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

absl::uint128 PowerMod(absl::uint128 base, absl::uint128 exp,
                       absl::uint128 mod) {
  absl::uint128 product = 1;
  absl::uint128 power_mult = base;
  for (int64_t bit = 1; bit <= exp; bit <<= 1) {
    if (exp & bit) product = (product * power_mult) % mod;
    power_mult = (power_mult * power_mult) % mod;
  }
  return product;
}

Transform IncrementNInverse(int64_t n, int64_t deck_size) {
  // TODO(@monkeynova): Error handling.
  CHECK(std::gcd(n, deck_size) == 1);
  // If 'n' and 'mod' are relatively prime, n ** (mod - 1) == 1, which means
  // n ** -1 == n ** (mod - 2).
  absl::uint128 inverse_n = PowerMod(n, deck_size - 2, deck_size);
  return Transform{.mult = inverse_n, .add = 0, .mod = deck_size};
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

absl::StatusOr<std::vector<std::string>> Day22_2019::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::pair<Transform, Transform>> pair =
      CreateTransform(input, /*deck_size=*/10007);
  if (!pair.ok()) return pair.status();

  auto [t, _] = *pair;
  return IntReturn(t.Apply(2019));
}

absl::StatusOr<std::vector<std::string>> Day22_2019::Part2(
    absl::Span<absl::string_view> input) const {
  int64_t run_count = 101741582076661;
  absl::StatusOr<std::pair<Transform, Transform>> pair =
      CreateTransform(input, /*deck_size=*/119315717514047);
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
