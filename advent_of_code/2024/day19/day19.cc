// http://adventofcode.com/2024/day/19

#include "advent_of_code/2024/day19/day19.h"

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
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool CanMake(std::string_view test,
             absl::Span<const std::string_view> patterns,
             absl::flat_hash_map<std::string_view, bool>& hist) {
  auto it = hist.find(test);
  if (it != hist.end()) return it->second;

  bool ret = false;
  for (std::string_view p : patterns) {
    if (test.substr(0, p.size()) != p) continue;
    if (!CanMake(test.substr(p.size()), patterns, hist)) continue;
    ret = true;
    break;
  }

  return hist[test] = ret;
}

class Patterns {
 public:
  Patterns(std::vector<std::string_view> patterns) {
    for (std::string_view p : patterns) {
      Prefix* val = &root_;
      for (char c : p) {
        auto [it, inserted] = val->subs.emplace(c, nullptr);
        if (inserted) {
          own.push_back(std::make_unique<Prefix>());
          it->second = own.back().get();
        }
        val = it->second;
      }
      val->match = true;
    }
  }

  void Foreach(std::string_view find,
               absl::FunctionRef<void(std::string_view)> fn) const {
    std::string build;
    int c_idx = 0;
    const Prefix* val = &root_;
    while (val != nullptr) {
      if (val->match) fn(build);
      if (c_idx == find.size()) break;
      auto it = val->subs.find(find[c_idx]);
      if (it == val->subs.end()) break;
      build.append(1, find[c_idx]);
      ++c_idx;
      val = it->second;
    }
  }

 private:
  struct Prefix {
    // TODO array<5>.
    bool match = false;
    absl::flat_hash_map<char, Prefix*> subs;

    std::string DebugString(std::string prefix = "") {
      std::string ret;
      absl::StrAppend(&ret, prefix, "match=", match, "\n");
      std::string sub_prefix = absl::StrCat(prefix, "  ");
      for (const auto [c, sub] : subs) {
        absl::StrAppend(&ret, prefix, "c=", std::string_view(&c, 1), "\n");
        absl::StrAppend(&ret, sub->DebugString(sub_prefix));
      }
      return ret;
    }
  };
  std::vector<std::unique_ptr<Prefix>> own;
  Prefix root_;
};

int64_t CountMakable(std::string_view test, const Patterns& patterns,
                     absl::flat_hash_map<std::string_view, int64_t>& hist) {
  if (auto it = hist.find(test); it != hist.end()) {
    return it->second;
  }
  
  int64_t ret = 0;
  patterns.Foreach(test, [&](std::string_view p) {
    ret += CountMakable(test.substr(p.size()), patterns, hist);
  });

  return hist[test] = ret;
}


}  // namespace

absl::StatusOr<std::string> Day_2024_19::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return absl::InvalidArgumentError("Too short");
  std::vector<std::string_view> patterns = absl::StrSplit(input[0], ", ");
  if (!input[1].empty()) return absl::InvalidArgumentError("No blank line");

  absl::flat_hash_map<std::string_view, bool> hist = {{"", true}};
  int count = 0;
  for (int i = 2; i < input.size(); ++i) {
    if (CanMake(input[i], patterns, hist)) {
      ++count;
    }
  }
  LOG(ERROR) << hist.size();
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2024_19::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return absl::InvalidArgumentError("Too short");
  std::vector<std::string_view> pattern_list = absl::StrSplit(input[0], ", ");
  if (!input[1].empty()) return absl::InvalidArgumentError("No blank line");

  absl::flat_hash_map<std::string_view, int64_t> hist = {{"", 1}};
  Patterns patterns(std::move(pattern_list));
  int64_t count = 0;
  for (int i = 2; i < input.size(); ++i) {
    count += CountMakable(input[i], patterns, hist);
  }
  LOG(ERROR) << hist.size();
  return AdventReturn(count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_19()); });

}  // namespace advent_of_code
