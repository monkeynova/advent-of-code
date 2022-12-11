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
  int64_t div_test;
  int64_t true_monkey;
  int64_t false_monkey;
  enum { kPlus = 1, kTimes = 2, kTimesOld = 3 } op;
  int64_t op_val;
  struct Item {
    int64_t worry;
  };
  std::vector<Item> items;

  int64_t inspections = 0;
};

}  // namespace

absl::StatusOr<std::string> Day_2022_11::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Monkey> monkeys;
  Monkey* cur_monkey = nullptr;
  for (absl::string_view line : input) {
    if (line.empty()) continue;
    int64_t i;
    absl::string_view str;
    if (RE2::FullMatch(line, "Monkey (\\d+):", &i)) {
      while (monkeys.size() <= i) {
        monkeys.push_back({});
      }
      cur_monkey = &monkeys[i];
    } else if (RE2::FullMatch(line, "  Starting items: ([\\d, ]+)", &str)) {
      for (absl::string_view item : absl::StrSplit(str, ",")) {
        item = absl::StripAsciiWhitespace(item);
        if (!absl::SimpleAtoi(item, &i)) {
          return Error("Bad item: ", item);
        }
        cur_monkey->items.push_back({.worry = i});
      }
    } else if (RE2::FullMatch(line, "  Operation: new = old \\+ (\\d+)", &i)) {
      cur_monkey->op = Monkey::kPlus;
      cur_monkey->op_val = i;
    } else if (RE2::FullMatch(line, "  Operation: new = old \\* old")) {
      cur_monkey->op = Monkey::kTimesOld;
    } else if (RE2::FullMatch(line, "  Operation: new = old \\* (\\d+)", &i)) {
      cur_monkey->op = Monkey::kTimes;
      cur_monkey->op_val = i;
    } else if (RE2::FullMatch(line, "  Test: divisible by (\\d+)", &i)) {
      cur_monkey->div_test = i;
    } else if (RE2::FullMatch(line, "    If true: throw to monkey (\\d+)", &i)) {
      cur_monkey->true_monkey = i;
    } else if (RE2::FullMatch(line, "    If false: throw to monkey (\\d+)", &i)) {
      cur_monkey->false_monkey = i;
    } else {
      return Error("Bad line: ", line);
    }
  }
  for (int64_t round = 0; round < 20; ++round) {
    for (Monkey& m : monkeys) {
      for (Monkey::Item& i : m.items) {
        if (m.op == Monkey::kPlus) i.worry += m.op_val;
        else if (m.op == Monkey::kTimes) i.worry *= m.op_val;
        else if (m.op == Monkey::kTimesOld) i.worry *= i.worry;
        else return Error("Bad op");
        i.worry /= 3;
        if (i.worry % m.div_test == 0) {
          monkeys[m.true_monkey].items.push_back(i);
        } else {
          monkeys[m.false_monkey].items.push_back(i);
        }
      }
      m.inspections += m.items.size();
      m.items.clear();
    }
  }
  absl::c_sort(monkeys, [](const Monkey& a, const Monkey& b) { return a.inspections > b.inspections; });
  return IntReturn(monkeys[0].inspections * monkeys[1].inspections);
}

absl::StatusOr<std::string> Day_2022_11::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Monkey> monkeys;
  Monkey* cur_monkey = nullptr;
  for (absl::string_view line : input) {
    if (line.empty()) continue;
    int64_t i;
    absl::string_view str;
    if (RE2::FullMatch(line, "Monkey (\\d+):", &i)) {
      while (monkeys.size() <= i) {
        monkeys.push_back({});
      }
      cur_monkey = &monkeys[i];
    } else if (RE2::FullMatch(line, "  Starting items: ([\\d, ]+)", &str)) {
      for (absl::string_view item : absl::StrSplit(str, ",")) {
        item = absl::StripAsciiWhitespace(item);
        if (!absl::SimpleAtoi(item, &i)) {
          return Error("Bad item: ", item);
        }
        cur_monkey->items.push_back({.worry = i});
      }
    } else if (RE2::FullMatch(line, "  Operation: new = old \\+ (\\d+)", &i)) {
      cur_monkey->op = Monkey::kPlus;
      cur_monkey->op_val = i;
    } else if (RE2::FullMatch(line, "  Operation: new = old \\* old")) {
      cur_monkey->op = Monkey::kTimesOld;
    } else if (RE2::FullMatch(line, "  Operation: new = old \\* (\\d+)", &i)) {
      cur_monkey->op = Monkey::kTimes;
      cur_monkey->op_val = i;
    } else if (RE2::FullMatch(line, "  Test: divisible by (\\d+)", &i)) {
      cur_monkey->div_test = i;
    } else if (RE2::FullMatch(line, "    If true: throw to monkey (\\d+)", &i)) {
      cur_monkey->true_monkey = i;
    } else if (RE2::FullMatch(line, "    If false: throw to monkey (\\d+)", &i)) {
      cur_monkey->false_monkey = i;
    } else {
      return Error("Bad line: ", line);
    }
  }
  int full_mod = 1;
  for (Monkey& m : monkeys) {
    full_mod *= m.div_test;
  }

  for (int64_t round = 0; round < 10000; ++round) {
    for (Monkey& m : monkeys) {
      for (Monkey::Item& i : m.items) {
        if (m.op == Monkey::kPlus) i.worry += m.op_val;
        else if (m.op == Monkey::kTimes) i.worry *= m.op_val;
        else if (m.op == Monkey::kTimesOld) i.worry *= i.worry;
        else return Error("Bad op");
        i.worry %= full_mod;
        if (i.worry % m.div_test == 0) {
          monkeys[m.true_monkey].items.push_back(i);
        } else {
          monkeys[m.false_monkey].items.push_back(i);
        }
      }
      m.inspections += m.items.size();
      m.items.clear();
    }
  }
  absl::c_sort(monkeys, [](const Monkey& a, const Monkey& b) { return a.inspections > b.inspections; });
  return IntReturn(monkeys[0].inspections * monkeys[1].inspections);
}

}  // namespace advent_of_code
