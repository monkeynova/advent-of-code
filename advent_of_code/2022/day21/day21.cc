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
  std::string_view left;
  std::string_view right;
  std::string_view op;
  bool unknown = false;

  friend std::ostream& operator<<(std::ostream& out, const Monkey& m) {
    if (m.unknown) return out << "x";
    if (m.value) return out << *m.value;
    return out << m.left << " " << m.op << " " << m.right;
  }
};

absl::StatusOr<int64_t> ConstantFold(
    absl::flat_hash_map<std::string_view, Monkey>& monkeys,
    std::string_view name) {
  VLOG(2) << "ConstantFold(" << name << ")";

  if (!monkeys.contains(name)) return Error("Bad monkey: ", name);
  Monkey& me = monkeys[name];
  if (me.unknown) return absl::NotFoundError("unknown");

  if (!me.value) {
    absl::StatusOr<int64_t> left = ConstantFold(monkeys, me.left);
    absl::StatusOr<int64_t> right = ConstantFold(monkeys, me.right);
    // Evaluate both sides before checking for errors, so that we evaluate
    // all that we can before we return a NotFound error for either branch.
    if (!left.ok()) return left.status();
    if (!right.ok()) return right.status();
    if (me.op == "+") {
      me.value = *left + *right;
    } else if (me.op == "-") {
      me.value = *left - *right;
    } else if (me.op == "*") {
      me.value = *left * *right;
    } else if (me.op == "/") {
      me.value = *left / *right;
    } else {
      LOG(FATAL) << "Bad op: " << me.op;
    }
  }
  VLOG(2) << name << " -> " << *me.value;

  return *me.value;
}

absl::StatusOr<int64_t> Evaluate(
    absl::flat_hash_map<std::string_view, Monkey>& monkeys,
    std::string_view name) {
  return ConstantFold(monkeys, name);
}

absl::StatusOr<int64_t> SolveForVal(
    absl::flat_hash_map<std::string_view, Monkey>& monkeys, int64_t val,
    std::string_view node) {
  if (!monkeys.contains(node)) return Error("Bad monkey: ", node);
  const Monkey& me = monkeys[node];
  if (me.unknown) return val;

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
      return SolveForVal(monkeys, val - *left.value, me.right);
    } else {
      return SolveForVal(monkeys, val - *right.value, me.left);
    }
  } else if (me.op == "-") {
    if (left.value) {
      return SolveForVal(monkeys, *left.value - val, me.right);
    } else {
      return SolveForVal(monkeys, val + *right.value, me.left);
    }
  } else if (me.op == "*") {
    if (left.value) {
      if (val % *left.value != 0) return Error("Non-integral division");
      return SolveForVal(monkeys, val / *left.value, me.right);
    } else {
      if (val % *right.value != 0) return Error("Non-integral division");
      return SolveForVal(monkeys, val / *right.value, me.left);
    }
  } else if (me.op == "/") {
    if (left.value) {
      if (*left.value % val != 0) return Error("Non-integral division");
      return SolveForVal(monkeys, *left.value / val, me.right);
    } else {
      return SolveForVal(monkeys, val * *right.value, me.left);
    }
  }

  return Error("Bad op: ", me.op);
}

bool OkOrNotFound(absl::Status st) {
  return st.ok() || st.code() == absl::StatusCode::kNotFound;
}

absl::StatusOr<int64_t> SolveForRootEquality(
    absl::flat_hash_map<std::string_view, Monkey>& monkeys) {
  if (!monkeys.contains("root")) return Error("No root");
  Monkey& root = monkeys["root"];
  absl::StatusOr<int64_t> left = ConstantFold(monkeys, root.left);
  absl::StatusOr<int64_t> right = ConstantFold(monkeys, root.right);
  if (!OkOrNotFound(left.status())) return left.status();
  if (!OkOrNotFound(right.status())) return right.status();

  if (left.ok() && right.ok()) return Error("Both sides are constant");
  if (!left.ok() && !right.ok()) return Error("Neigher side is constant");
  if (left.ok()) return SolveForVal(monkeys, *left, root.right);
  return SolveForVal(monkeys, *right, root.left);
}

absl::StatusOr<absl::flat_hash_map<std::string_view, Monkey>> ParseMonkeys(
    absl::Span<std::string_view> input) {
  absl::flat_hash_map<std::string_view, Monkey> monkeys;
  for (std::string_view line : input) {
    Monkey m;
    std::string_view name;
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
  return monkeys;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_21::Part1(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string_view, Monkey> monkeys;
  ASSIGN_OR_RETURN(monkeys, ParseMonkeys(input));
  return AdventReturn(Evaluate(monkeys, "root"));
}

absl::StatusOr<std::string> Day_2022_21::Part2(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string_view, Monkey> monkeys;
  ASSIGN_OR_RETURN(monkeys, ParseMonkeys(input));

  auto humn_it = monkeys.find("humn");
  if (humn_it == monkeys.end()) return Error("No human");
  Monkey& humn = humn_it->second;
  humn.unknown = true;
  humn.value = std::nullopt;

  return AdventReturn(SolveForRootEquality(monkeys));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/21, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_21());
});

}  // namespace advent_of_code
