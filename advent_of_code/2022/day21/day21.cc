// http://adventofcode.com/2022/day/21

#include "advent_of_code/2022/day21/day21.h"

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
  std::optional<int64_t> value;
  absl::string_view left;
  absl::string_view right;
  absl::string_view op;
};

int64_t Evaluate(absl::flat_hash_map<absl::string_view, Monkey>& monkeys, absl::string_view name) {
  auto it = monkeys.find(name);
  CHECK(it != monkeys.end());
  if (!it->second.value) {
    int64_t left = Evaluate(monkeys, it->second.left);
    int64_t right = Evaluate(monkeys, it->second.right);
    VLOG(2) << left << " ? " << right;
    if (it->second.op == "+") {
      it->second.value = left + right;
    } else if (it->second.op == "-") {
      it->second.value = left - right;
    } else if (it->second.op == "*") {
      it->second.value = left * right;
    } else if (it->second.op == "/") {
      it->second.value = left / right;
    } else {
      LOG(FATAL) << "Bad op: " << it->second.op;
    }
    VLOG(2) << *it->second.value;
    VLOG(2) << "  " << it->second.left << " " << it->second.op << " " << it->second.right;
  }
  VLOG(2) << name << " -> " << *it->second.value;

  return *it->second.value;
}

std::optional<int64_t> ConstantFold(absl::flat_hash_map<absl::string_view, Monkey>& monkeys, absl::string_view name) {
  if (name == "humn") return std::nullopt;

  VLOG(2) << "ConstantFold(" << name << ")";

  auto it = monkeys.find(name);
  CHECK(it != monkeys.end());
  if (!it->second.value) {
    std::optional<int64_t> left = ConstantFold(monkeys, it->second.left);
    std::optional<int64_t> right = ConstantFold(monkeys, it->second.right);
    if (!left || !right) return std::nullopt;
    if (it->second.op == "+") {
      it->second.value = *left + *right;
    } else if (it->second.op == "-") {
      it->second.value = *left - *right;
    } else if (it->second.op == "*") {
      it->second.value = *left * *right;
    } else if (it->second.op == "/") {
      it->second.value = *left / *right;
    } else {
      LOG(FATAL) << "Bad op: " << it->second.op;
    }
  }
  VLOG(2) << name << " -> " << *it->second.value;

  return *it->second.value;
}

int64_t ProbeVal(absl::flat_hash_map<absl::string_view, Monkey>& monkeys, int64_t val, absl::string_view node) {
  if (node == "humn") return val;
  Monkey me = monkeys[node];
  VLOG(2) << "ProbeVal: " << node << ": " << val << " = " << me.left << " " << me.op << " " << me.right;
  CHECK(!me.value);
  Monkey left = monkeys[me.left];
  Monkey right = monkeys[me.right];
  CHECK(left.value || right.value && !(left.value && right.value));
  if (me.op == "+") {
    if (left.value) {
      return ProbeVal(monkeys, val - *left.value, me.right);
    } else {
      return ProbeVal(monkeys, val - *right.value, me.left);
    }
  } else if (me.op == "-") {
    if (left.value) {
      return ProbeVal(monkeys, *left.value - val, me.right);
    } else {
      return ProbeVal(monkeys, val + *right.value, me.left);
    }    
  } else if (me.op == "*") {
    if (left.value) {
      return ProbeVal(monkeys, val / *left.value, me.right);
    } else {
      return ProbeVal(monkeys, val / *right.value, me.left);
    }    
  } else if (me.op == "/") {
    if (left.value) {
      return ProbeVal(monkeys, *left.value / val, me.right);
    } else {
      return ProbeVal(monkeys, val * *right.value, me.left);
    }        
  } else {
    LOG(FATAL) << "Bad op";
  }
}

std::string Tree(absl::flat_hash_map<absl::string_view, Monkey>& monkeys, absl::string_view node) {
  if (node == "humn") return "HUMN";
  CHECK(monkeys.contains(node)) << node;
  Monkey m = monkeys[node];
  if (m.value) return absl::StrCat(*m.value);
  return absl::StrCat("(", Tree(monkeys, m.left), m.op, Tree(monkeys, m.right), ")");
}

int64_t FindRoot(absl::flat_hash_map<absl::string_view, Monkey>& monkeys) {
  Monkey root = monkeys["root"];
  std::optional<int64_t> left = ConstantFold(monkeys, root.left);
  VLOG(1) << "left: " << Tree(monkeys, root.left);
  std::optional<int64_t> right = ConstantFold(monkeys, root.right);
  VLOG(1) << "right: " << Tree(monkeys, root.right);
  CHECK(left || right && !(left && right));
  if (left) {
    VLOG(1) << "left defined going to : " <<  root.right;
    return ProbeVal(monkeys, *left, root.right);
  }
  CHECK(right);
    VLOG(1) << "right defined going to : " <<  root.left;
  return ProbeVal(monkeys, *right, root.left);
}

}  // namespace

absl::StatusOr<std::string> Day_2022_21::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, Monkey> monkeys;
  for (absl::string_view line : input) {
    Monkey m;
    absl::string_view name;
    int64_t value;
    if (RE2::FullMatch(line, "([a-z]+): ([a-z]+) ([\\+\\-\\*\\/]) ([a-z]+)",
         &name, &m.left, &m.op, &m.right)) {
      monkeys.insert({name, m});
    } else if (RE2::FullMatch(line, "([a-z]+): ([-\\d]+)", &name, &value)) {
      m.value = value;
      monkeys.insert({name, m});
    } else {
      return Error("Bad line: ", line);
    }
  }
  return IntReturn(Evaluate(monkeys, "root"));
}

absl::StatusOr<std::string> Day_2022_21::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, Monkey> monkeys;
  for (absl::string_view line : input) {
    Monkey m;
    absl::string_view name;
    int64_t value;
    if (RE2::FullMatch(line, "([a-z]+): ([a-z]+) ([\\+\\-\\*\\/]) ([a-z]+)",
         &name, &m.left, &m.op, &m.right)) {
      monkeys.insert({name, m});
    } else if (RE2::FullMatch(line, "([a-z]+): ([-\\d]+)", &name, &value)) {
      if (name != "humn") m.value = value;
      monkeys.insert({name, m});
    } else {
      return Error("Bad line: ", line);
    }
  }

  return IntReturn(FindRoot(monkeys));

  absl::string_view left = monkeys["root"].left;
  absl::string_view right = monkeys["root"].right;
  for (int i = 0; true; ++i) {
    absl::flat_hash_map<absl::string_view, Monkey> clean = monkeys;
    clean["humn"].value = i;
    if (Evaluate(clean, left) == Evaluate(clean, right)) return IntReturn(i);
  }

  return Error("left inifinite loop");
}

}  // namespace advent_of_code
