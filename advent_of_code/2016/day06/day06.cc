#include "advent_of_code/2016/day06/day06.h"

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

}  // namespace

absl::StatusOr<std::string> Day_2016_06::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::flat_hash_map<char, int>> freqs;
  if (input.empty()) return Error("Bad Input");
  freqs.resize(input[0].size());
  for (absl::string_view txt : input) {
    if (freqs.size() != txt.size()) return Error("Mismatch input");
    for (int i = 0; i < txt.size(); ++i) {
      ++freqs[i][txt[i]];
    }
  }
  std::string out;
  out.resize(freqs.size());
  for (int i = 0; i < freqs.size(); ++i) {
    char max_char = '\0';
    int max_count = -1;
    for (const auto& [c, count] : freqs[i]) {
      if (count > max_count) {
        max_count = count;
        max_char = c;
      }
    }
    out[i] = max_char;
  }
  return out;
}

absl::StatusOr<std::string> Day_2016_06::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::flat_hash_map<char, int>> freqs;
  if (input.empty()) return Error("Bad Input");
  freqs.resize(input[0].size());
  for (absl::string_view txt : input) {
    if (freqs.size() != txt.size()) return Error("Mismatch input");
    for (int i = 0; i < txt.size(); ++i) {
      ++freqs[i][txt[i]];
    }
  }
  std::string out;
  out.resize(freqs.size());
  for (int i = 0; i < freqs.size(); ++i) {
    char min_char = '\0';
    int min_count = std::numeric_limits<int>::max();
    for (const auto& [c, count] : freqs[i]) {
      if (count < min_count) {
        min_count = count;
        min_char = c;
      }
    }
    out[i] = min_char;
  }
  return out;
}

}  // namespace advent_of_code
