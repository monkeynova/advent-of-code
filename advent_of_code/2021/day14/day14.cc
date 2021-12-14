#include "advent_of_code/2021/day14/day14.h"

#include "absl/algorithm/container.h"
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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_14::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view start = input[0];
  if (input[1] != "") return Error("Bad input");
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
  for (int i = 2; i < input.size(); ++i) {
    auto [pair, insert] = PairSplit(input[i], " -> ");
    rules.emplace(pair, insert);
  }
  std::string rec = std::string(start);
  for (int i = 0; i < 10; ++i) {
    absl::string_view rec_view = rec;
    std::string next;
    for (int j = 0; j < rec.size() - 1; ++j) {
      next.append(rec_view.substr(j, 1));
      auto it = rules.find(rec_view.substr(j, 2));
      if (it == rules.end()) return Error("Could not rule");
      next.append(it->second);
    }
    next.append(1, rec_view.back());
    rec = next;
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
  if (input.size() < 3) return Error("Bad input");
  absl::string_view start = input[0];
  if (input[1] != "") return Error("Bad input");
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
  for (int i = 2; i < input.size(); ++i) {
    auto [pair, insert] = PairSplit(input[i], " -> ");
    rules.emplace(pair, insert);
  }
  
  absl::flat_hash_map<std::string, int64_t> pair_counts;
  for (int i = 0; i < start.size() - 1; ++i) {
    ++pair_counts[std::string(start.substr(i, 2))];
  }
  for (int i = 0; i < 40; ++i) {
    //VLOG(1) << i;
    absl::flat_hash_map<std::string, int64_t> new_pair_counts;
    for (const auto& [str, count] : pair_counts) {
      //VLOG(1) << "  " << str << ": " << count;
      auto it = rules.find(str);
      if (it == rules.end()) return Error("Bad");
      std::string left = {str[0], it->second[0]};
      std::string right = {it->second[0], str[1]};
      new_pair_counts[left] += count;
      new_pair_counts[right] += count;
    }
    pair_counts = std::move(new_pair_counts);
  }
  absl::flat_hash_map<std::string, int64_t> counts;
  for (const auto& [str, count] : pair_counts) {
    counts[str.substr(0, 1)] += count;
    counts[str.substr(1, 1)] += count;
  }
  for (auto& [str, count] : pair_counts) {
  }

  int64_t max = 0;
  int64_t min = std::numeric_limits<int64_t>::max();
  for (const auto& [str, c] : counts) {
    int64_t count = c;
    if (str == start.substr(0, 1) || str == start.substr(start.size() - 1, 1)) {
      ++count;
    }
    count /= 2;
    VLOG(1) << str << ": " << count;
    if (count > max) max = count;
    if (count < min) min = count;
  }
  return IntReturn(max - min);
}

}  // namespace advent_of_code
