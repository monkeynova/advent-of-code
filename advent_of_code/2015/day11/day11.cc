#include "advent_of_code/2015/day11/day11.h"

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

void Increment(std::string* password) {
  for (int i = password->size() - 1; i >= 0; --i) {
    if (++(*password)[i] != 'z' + 1) break;
    (*password)[i] = 'a';
  }
}

bool IsValid(absl::string_view password) {
  static absl::flat_hash_set<char> kBadChars = {'i', 'o', 'l'};
  bool has_inc_triple = false;
  bool has_two_pairs = false;
  absl::string_view last_pair;
  for (int i = 0; i < password.size(); ++i) {
    // TODO(@monkeynova): Push to Increment.
    if (kBadChars.contains(password[i])) return false;
    if (i + 1 < password.size()) {
      if (password[i] == password[i + 1]) {
        if (last_pair.empty()) {
          last_pair = password.substr(i, 2);
        } else if (password.substr(i, 2) != last_pair) {
          has_two_pairs = true;
        }
      }
    }
    if (i + 2 < password.size()) {
      if (password[i] == password[i + 1] - 1 &&
          password[i] == password[i + 2] - 2) {
        has_inc_triple = true;
      }
    }
  }
  if (!has_inc_triple) return false;
  if (!has_two_pairs) return false;

  return true;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_11::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string password = std::string(input[0]);
  do {
    Increment(&password);
  } while (!IsValid(password));

  return password;
}

absl::StatusOr<std::string> Day_2015_11::Part2(
    absl::Span<absl::string_view> input) const {
  std::string password = std::string(input[0]);
  do {
    Increment(&password);
  } while (!IsValid(password));
  do {
    Increment(&password);
  } while (!IsValid(password));

  return password;
}

}  // namespace advent_of_code
