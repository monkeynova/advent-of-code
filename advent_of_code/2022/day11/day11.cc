// http://adventofcode.com/2022/day/11

#include "advent_of_code/2022/day11/day11.h"

#include "absl/algorithm/container.h"
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

struct Monkey {
  int div_test = -1;
  int true_monkey = -1;
  int false_monkey = -1;
  enum { kUnset = 0, kPlus = 1, kTimes = 2, kTimesOld = 3 } op = kUnset;
  int op_val = -1;
  struct Item {
    int64_t worry;
  };
  std::vector<Item> items;

  int inspections = 0;

  bool IsValid() const {
    if (div_test == -1) return false;
    if (true_monkey == -1) return false;
    if (false_monkey == -1) return false;
    if (op == kUnset) return false;
    if (op_val == -1 && op != kTimesOld) return false;
    return true;
  }

  int NextMonkey(const Item& i) const {
    return i.worry % div_test == 0 ? true_monkey : false_monkey;
  }

  void UpdateWorry(Item& i) const {
    if (op == Monkey::kPlus) {
      i.worry += op_val;
    } else if (op == Monkey::kTimes) {
      i.worry *= op_val;
    } else if (op == Monkey::kTimesOld) {
      i.worry *= i.worry;
    } else {
      LOG(FATAL) << "Bad op";
    }
  }
};

absl::StatusOr<std::vector<Monkey>> ParseMonkeys(
    absl::Span<absl::string_view> input) {
  std::vector<Monkey> monkeys;
  Monkey* cur_monkey = nullptr;
  for (absl::string_view line : input) {
    if (line.empty()) {
      if (cur_monkey != nullptr && !cur_monkey->IsValid()) {
        return Error("Invalid monkey");
      }
      cur_monkey = nullptr;
      continue;
    }
    int i;
    absl::string_view str;
    if (RE2::FullMatch(line, "Monkey (\\d+):", &i)) {
      if (monkeys.size() != i) return Error("Bad monkey order");
      monkeys.push_back({});
      cur_monkey = &monkeys[i];

    } else if (RE2::FullMatch(line, "  Starting items: ([\\d, ]+)", &str)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      for (absl::string_view item : absl::StrSplit(str, ",")) {
        item = absl::StripAsciiWhitespace(item);
        if (!absl::SimpleAtoi(item, &i)) {
          return Error("Bad item: ", item);
        }
        cur_monkey->items.push_back({.worry = i});
      }

    } else if (RE2::FullMatch(line, "  Operation: new = old \\+ (\\d+)", &i)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->op = Monkey::kPlus;
      cur_monkey->op_val = i;

    } else if (RE2::FullMatch(line, "  Operation: new = old \\* old")) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->op = Monkey::kTimesOld;

    } else if (RE2::FullMatch(line, "  Operation: new = old \\* (\\d+)", &i)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->op = Monkey::kTimes;
      cur_monkey->op_val = i;

    } else if (RE2::FullMatch(line, "  Test: divisible by (\\d+)", &i)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->div_test = i;

    } else if (RE2::FullMatch(line, "    If true: throw to monkey (\\d+)",
                              &i)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->true_monkey = i;

    } else if (RE2::FullMatch(line, "    If false: throw to monkey (\\d+)",
                              &i)) {
      if (cur_monkey == nullptr) return Error("No monkey");
      cur_monkey->false_monkey = i;

    } else {
      return Error("Bad line: ", line);
    }
  }
  return monkeys;
}

int64_t ScoreMonkeys(const std::vector<Monkey>& monkeys) {
  // We only need the top two. Don't bother with a full sort, or a priority
  // heap when a very simple, unrolled insertion sort will do just fine.
  // H/T to @cshabsin for this trick.
  int64_t top2[2] = {0, 0};
  for (const Monkey& m : monkeys) {
    if (m.inspections > top2[0]) {
      top2[1] = top2[0];
      top2[0] = m.inspections;
    } else if (m.inspections > top2[1]) {
      top2[1] = m.inspections;
    }
  }
  return top2[0] * top2[1];
}

}  // namespace

absl::StatusOr<std::string> Day_2022_11::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Monkey>> monkeys = ParseMonkeys(input);
  if (!monkeys.ok()) return monkeys.status();

  for (int64_t round = 0; round < 20; ++round) {
    for (Monkey& m : *monkeys) {
      for (Monkey::Item& i : m.items) {
        m.UpdateWorry(i);
        i.worry /= 3;
        (*monkeys)[m.NextMonkey(i)].items.push_back(i);
      }
      m.inspections += m.items.size();
      m.items.clear();
    }
  }
  return AdventReturn(ScoreMonkeys(*monkeys));
}

absl::StatusOr<std::string> Day_2022_11::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Monkey>> monkeys = ParseMonkeys(input);
  if (!monkeys.ok()) return monkeys.status();

  int full_mod = 1;
  for (const Monkey& m : *monkeys) full_mod *= m.div_test;
  VLOG(1) << "full_mod = " << full_mod;

  for (int64_t round = 0; round < 10'000; ++round) {
    for (Monkey& m : *monkeys) {
      for (Monkey::Item& i : m.items) {
        m.UpdateWorry(i);
        i.worry %= full_mod;
        (*monkeys)[m.NextMonkey(i)].items.push_back(i);
      }
      m.inspections += m.items.size();
      m.items.clear();
    }
  }
  return AdventReturn(ScoreMonkeys(*monkeys));
}

}  // namespace advent_of_code
