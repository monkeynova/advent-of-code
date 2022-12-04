// http://adventofcode.com/2021/day/14

#include "advent_of_code/2021/day14/day14.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Problem {
  absl::string_view start;
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
};

using CharPair = std::array<char, 2>;
using CharPairCounts = absl::flat_hash_map<CharPair, int64_t>;

absl::StatusOr<Problem> Parse(absl::Span<absl::string_view> input) {
  Problem ret;
  if (input.size() < 3) return Error("Bad input");
  ret.start = input[0];
  if (input[1] != "") return Error("Bad input");
  for (int i = 2; i < input.size(); ++i) {
    auto [pair, insert] = PairSplit(input[i], " -> ");
    if (pair.size() != 2) return Error("Bad from: ", input[i]);
    if (insert.size() != 1) return Error("Bad insert: ", input[i]);
    ret.rules.emplace(pair, insert);
  }
  return ret;
}

absl::StatusOr<std::string> Expand(absl::string_view rec, const Problem& p) {
  std::string next;
  for (int j = 0; j < rec.size() - 1; ++j) {
    next.append(rec.substr(j, 1));
    auto it = p.rules.find(rec.substr(j, 2));
    if (it == p.rules.end()) return Error("Could not rule");
    next.append(it->second);
  }
  next.append(1, rec.back());
  return next;
}

absl::StatusOr<CharPairCounts> Expand(const CharPairCounts& pair_counts,
                                      const Problem& p) {
  CharPairCounts new_pair_counts;
  for (const auto& [str, count] : pair_counts) {
    auto it = p.rules.find(absl::string_view(str.data(), 2));
    if (it == p.rules.end()) return Error("Bad");
    CharPair left = {str[0], it->second[0]};
    CharPair right = {it->second[0], str[1]};
    new_pair_counts[left] += count;
    new_pair_counts[right] += count;
  }
  return new_pair_counts;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_14::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Problem> p = Parse(input);
  if (!p.ok()) return p.status();

  std::string rec = std::string(p->start);
  for (int i = 0; i < 10; ++i) {
    absl::StatusOr<std::string> next = Expand(rec, *p);
    if (!next.ok()) return next.status();
    rec = std::move(*next);
  }
  absl::flat_hash_map<absl::string_view, int64_t> counts;
  absl::string_view rec_view = rec;
  for (int i = 0; i < rec.size(); ++i) {
    ++counts[rec_view.substr(i, 1)];
  }
  int64_t max = 0;
  int64_t min = rec.size();
  for (const auto& [_, c] : counts) {
    if (c > max) max = c;
    if (c < min) min = c;
  }
  return IntReturn(max - min);
}

absl::StatusOr<std::string> Day_2021_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Problem> p = Parse(input);
  if (!p.ok()) return p.status();

  CharPairCounts pair_counts;
  for (int i = 0; i < p->start.size() - 1; ++i) {
    ++pair_counts[{p->start[i], p->start[i + 1]}];
  }
  for (int i = 0; i < 40; ++i) {
    absl::StatusOr<CharPairCounts> new_pair_counts = Expand(pair_counts, *p);
    pair_counts = std::move(*new_pair_counts);
  }
  absl::flat_hash_map<char, int64_t> single_char_counts = {
      // End characters missed one count in pairs.
      {p->start[0], 1},
      {p->start.back(), 1},
  };
  for (const auto& [str, count] : pair_counts) {
    single_char_counts[str[0]] += count;
    single_char_counts[str[1]] += count;
  }

  int64_t max = 0;
  int64_t min = std::numeric_limits<int64_t>::max();
  for (auto [_, count] : single_char_counts) {
    // All characters counted twice (once in each pair).
    if (count % 2 != 0) return Error("MOD 2");
    count /= 2;
    if (count > max) max = count;
    if (count < min) min = count;
  }
  return IntReturn(max - min);
}

}  // namespace advent_of_code
