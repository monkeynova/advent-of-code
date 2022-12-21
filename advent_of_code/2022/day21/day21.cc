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

  friend std::ostream& operator<<(std::ostream& out, const Monkey& m) {
    if (m.value) return out << *m.value;
    return out << m.left << " " << m.op << " " << m.right;
  }
};

absl::StatusOr<int64_t> Evaluate(
    absl::flat_hash_map<absl::string_view, Monkey>& monkeys,
    absl::string_view name) {
  if (!monkeys.contains(name)) return Error("Bad monkey: ", name);
  Monkey& me = monkeys[name];
  if (!me.value) {
    absl::StatusOr<int64_t> left = Evaluate(monkeys, me.left);
    if (!left.ok()) return left.status();
    absl::StatusOr<int64_t> right = Evaluate(monkeys, me.right);
    if (!right.ok()) return right.status();
    VLOG(2) << *left << " ? " << *right;
    if (me.op == "+") {
      me.value = *left + *right;
    } else if (me.op == "-") {
      me.value = *left - *right;
    } else if (me.op == "*") {
      me.value = *left * *right;
    } else if (me.op == "/") {
      me.value = *left / *right;
    } else {
      return Error("Bad op: ", me.op, " (in monkey ", name, ")");
    }
    VLOG(2) << *me.value;
    VLOG(2) << "  " << me;
  }
  VLOG(2) << name << " -> " << *me.value;

  return *me.value;
}

absl::StatusOr<std::optional<int64_t>> ConstantFold(
    absl::flat_hash_map<absl::string_view, Monkey>& monkeys,
    absl::string_view name) {
  if (name == "humn") return std::nullopt;

  VLOG(2) << "ConstantFold(" << name << ")";

  if (!monkeys.contains(name)) return Error("Bad monkey: ", name);
  Monkey& me = monkeys[name];
  if (!me.value) {
    absl::StatusOr<std::optional<int64_t>> left = ConstantFold(monkeys, me.left);
    if (!left.ok()) return left.status();
    absl::StatusOr<std::optional<int64_t>> right = ConstantFold(monkeys, me.right);
    if (!right.ok()) return right.status();
    if (!*left || !*right) return std::nullopt;
    if (me.op == "+") {
      me.value = **left + **right;
    } else if (me.op == "-") {
      me.value = **left - **right;
    } else if (me.op == "*") {
      me.value = **left * **right;
    } else if (me.op == "/") {
      me.value = **left / **right;
    } else {
      LOG(FATAL) << "Bad op: " << me.op;
    }
  }
  VLOG(2) << name << " -> " << *me.value;

  return *me.value;
}

absl::StatusOr<int64_t> ProbeVal(
    absl::flat_hash_map<absl::string_view, Monkey>& monkeys,
    int64_t val, absl::string_view node) {
  if (node == "humn") return val;
  if (!monkeys.contains(node)) return Error("Bad monkey: ", node);
  const Monkey& me = monkeys[node];
  VLOG(2) << "ProbeVal: " << node << ": " << val << " = " << me;
  if (me.value) return Error("Probing known value for ", node);
  const Monkey& left = monkeys[me.left];
  const Monkey& right = monkeys[me.right];
  if (left.value && right.value) {
    return Error("Both left and right known for ", node);
  }
  if (!left.value && !right.value) {
    return Error("Neither left nor right known for ", node);
  }
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
  } 
  
  return Error("Bad op: ", me.op);
}

absl::StatusOr<int64_t> FindRoot(
    absl::flat_hash_map<absl::string_view, Monkey>& monkeys) {
  Monkey root = monkeys["root"];
  absl::StatusOr<std::optional<int64_t>> left =
      ConstantFold(monkeys, root.left);
  if (!left.ok()) return left.status();
  absl::StatusOr<std::optional<int64_t>> right =
      ConstantFold(monkeys, root.right);
  if (!right.ok()) return right.status();
  if (*left && *right) return Error("Both sides are constant");
  if (!*left && !*right) return Error("Neigher side is constant");
  if (*left) return ProbeVal(monkeys, **left, root.right);
  return ProbeVal(monkeys, **right, root.left);
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
}

}  // namespace advent_of_code
