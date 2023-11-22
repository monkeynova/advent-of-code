#include "advent_of_code/2016/day16/day16.h"

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

class DragonCurve {
 public:
  static void ExpandTo(std::string& str, int size) {
    while (str.size() != size) {
      int start_size = str.size();
      str.resize(2 * str.size() + 1, '0');
      for (int i = 0; i < start_size; ++i) {
        // ^1: 0 -> 1, 1 -> 0.
        str[str.size() - 1 - i] = str[i] ^ 1;
      }
      if (str.size() > size) str.resize(size);
      VLOG(2) << str;
    }
  }

  static void Checksum(std::string& str) {
    while (str.size() % 2 == 0) {
      for (int i = 0; i + 1 < str.size(); i += 2) {
        if (str[i] == str[i + 1]) {
          str[i / 2] = '1';
        } else {
          str[i / 2] = '0';
        }
      }
      VLOG(2) << str;
      str.resize(str.size() / 2);
    }
  }

  explicit DragonCurve(std::string_view str) : str_(str) {}

  char CharAt(int p) {
    // TODO(@monkeynova) We could be more efficient by calculating the needed
    // char on demand for it's position and doing the checksum in slices
    // of size = 2 ** N.
    return '\0';
    // size
    // 2 * size + 1
    // 4 * size + 3
    // 8 * size + 7
    // ...
    // 2 ** N * size + (2 ** N - 1)
    // 2 ** (N+1) * size + (2 ** (N+1) - 1)
  }

 private:
  std::string_view str_;
};

std::string RunPart1(std::string_view input, int size) {
  std::string s = std::string(input);
  int reserve = input.size();
  while (reserve < size) reserve = 2 * reserve + 1;
  s.reserve(reserve);
  VLOG(1) << "Start: " << s;
  DragonCurve::ExpandTo(s, size);
  VLOG(1) << "Expanded: " << s;
  DragonCurve::Checksum(s);
  VLOG(1) << "Checksum: " << s;
  return s;
}

}  // namespace

absl::StatusOr<std::string> Day_2016_16::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  ASSIGN_OR_RETURN(int val, IntParam());
  return RunPart1(input[0], val);
}

absl::StatusOr<std::string> Day_2016_16::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  ASSIGN_OR_RETURN(int val, IntParam());
  return RunPart1(input[0], val);
}

}  // namespace advent_of_code
