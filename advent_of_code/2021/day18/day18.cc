// https://adventofcode.com/2021/day/18
//
// --- Day 18: Snailfish ---
// -------------------------
//
// You descend into the ocean trench and encounter some snailfish. They
// say they saw the sleigh keys! They'll even tell you which direction
// the keys went if you help one of the smaller snailfish with his math
// homework.
//
// Snailfish numbers aren't like regular numbers. Instead, every
// snailfish number is a pair - an ordered list of two elements. Each
// element of the pair can be either a regular number or another pair.
//
// Pairs are written as [x,y], where x and y are the elements within the
// pair. Here are some example snailfish numbers, one snailfish number
// per line:
//
// [1,2]
// [[1,2],3]
// [9,[8,7]]
// [[1,9],[8,5]]
// [[[[1,2],[3,4]],[[5,6],[7,8]]],9]
// [[[9,[3,8]],[[0,9],6]],[[[3,7],[4,9]],3]]
// [[[[1,3],[5,3]],[[1,3],[8,7]]],[[[4,9],[6,9]],[[8,2],[7,3]]]]
//
// This snailfish homework is about addition. To add two snailfish
// numbers, form a pair from the left and right parameters of the
// addition operator. For example, [1,2] + [[3,4],5] becomes
// [[1,2],[[3,4],5]].
//
// There's only one problem: snailfish numbers must always be reduced,
// and the process of adding two snailfish numbers can result in
// snailfish numbers that need to be reduced.
//
// To reduce a snailfish number, you must repeatedly do the first action
// in this list that applies to the snailfish number:
//
// * If any pair is nested inside four pairs, the leftmost such pair
// explodes.
//
// * If any regular number is 10 or greater, the leftmost such regular
// number splits.
//
// Once no action in the above list applies, the snailfish number is
// reduced.
//
// During reduction, at most one action applies, after which the process
// returns to the top of the list of actions. For example, if split
// produces a pair that meets the explode criteria, that pair explodes
// before other splits occur.
//
// To explode a pair, the pair's left value is added to the first regular
// number to the left of the exploding pair (if any), and the pair's
// right value is added to the first regular number to the right of the
// exploding pair (if any). Exploding pairs will always consist of two
// regular numbers. Then, the entire exploding pair is replaced with the
// regular number 0.
//
// Here are some examples of a single explode action:
//
// * [[[[[9,8],1],2],3],4] becomes [[[[0,9],2],3],4] (the 9 has no
// regular number to its left, so it is not added to any regular
// number).
//
// * [7,[6,[5,[4,[3,2]]]]] becomes [7,[6,[5,[7,0]]]] (the 2 has no
// regular number to its right, and so it is not added to any regular
// number).
//
// * [[6,[5,[4,[3,2]]]],1] becomes [[6,[5,[7,0]]],3].
//
// * [[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]] becomes
// [[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]] (the pair [3,2] is unaffected because the
// pair [7,3] is further to the left; [3,2] would explode on the next action).
//
// * [[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]] becomes [[3,[2,[8,0]]],[9,[5,[7,0]]]].
//
// To split a regular number, replace it with a pair; the left element of
// the pair should be the regular number divided by two and rounded down,
// while the right element of the pair should be the regular number
// divided by two and rounded up. For example, 10 becomes [5,5], 11
// becomes [5,6], 12 becomes [6,6], and so on.
//
// Here is the process of finding the reduced result of
// [[[[4,3],4],4],[7,[[8,4],9]]] + [1,1]:
//
// after addition: [[[[   [4,3]   ,4],4],[7,[[8,4],9]]],[1,1]]
// after explode:  [[[[0,7],4],[7,[   [8,4]   ,9]]],[1,1]]
// after explode:  [[[[0,7],4],[   15   ,[0,13]]],[1,1]]
// after split:    [[[[0,7],4],[[7,8],[0,   13   ]]],[1,1]]
// after split:    [[[[0,7],4],[[7,8],[0,   [6,7]   ]]],[1,1]]
// after explode:  [[[[0,7],4],[[7,8],[6,0]]],[8,1]]
//
// Once no reduce actions apply, the snailfish number that remains is the
// actual result of the addition operation:
// [[[[0,7],4],[[7,8],[6,0]]],[8,1]].
//
// The homework assignment involves adding up a list of snailfish numbers
// (your puzzle input). The snailfish numbers are each listed on a
// separate line. Add the first snailfish number and the second, then add
// that result and the third, then add that result and the fourth, and so
// on until all numbers in the list have been used once.
//
// For example, the final sum of this list is
// [[[[1,1],[2,2]],[3,3]],[4,4]]:
//
// [1,1]
// [2,2]
// [3,3]
// [4,4]
//
// The final sum of this list is [[[[3,0],[5,3]],[4,4]],[5,5]]:
//
// [1,1]
// [2,2]
// [3,3]
// [4,4]
// [5,5]
//
// The final sum of this list is [[[[5,0],[7,4]],[5,5]],[6,6]]:
//
// [1,1]
// [2,2]
// [3,3]
// [4,4]
// [5,5]
// [6,6]
//
// Here's a slightly larger example:
//
// [[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]
// [7,[[[3,7],[4,3]],[[6,3],[8,8]]]]
// [[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]
// [[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]
// [7,[5,[[3,8],[1,4]]]]
// [[2,[2,2]],[8,[8,1]]]
// [2,9]
// [1,[[[9,3],9],[[9,0],[0,7]]]]
// [[[5,[7,4]],7],1]
// [[[[4,2],2],6],[8,7]]
//
// The final sum [[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]] is
// found after adding up the above snailfish numbers:
//
// [[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]
// + [7,[[[3,7],[4,3]],[[6,3],[8,8]]]]
// = [[[[4,0],[5,4]],[[7,7],[6,0]]],[[8,[7,7]],[[7,9],[5,0]]]]
//
// [[[[4,0],[5,4]],[[7,7],[6,0]]],[[8,[7,7]],[[7,9],[5,0]]]]
// + [[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]
// = [[[[6,7],[6,7]],[[7,7],[0,7]]],[[[8,7],[7,7]],[[8,8],[8,0]]]]
//
// [[[[6,7],[6,7]],[[7,7],[0,7]]],[[[8,7],[7,7]],[[8,8],[8,0]]]]
// + [[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]
// = [[[[7,0],[7,7]],[[7,7],[7,8]]],[[[7,7],[8,8]],[[7,7],[8,7]]]]
//
// [[[[7,0],[7,7]],[[7,7],[7,8]]],[[[7,7],[8,8]],[[7,7],[8,7]]]]
// + [7,[5,[[3,8],[1,4]]]]
// = [[[[7,7],[7,8]],[[9,5],[8,7]]],[[[6,8],[0,8]],[[9,9],[9,0]]]]
//
// [[[[7,7],[7,8]],[[9,5],[8,7]]],[[[6,8],[0,8]],[[9,9],[9,0]]]]
// + [[2,[2,2]],[8,[8,1]]]
// = [[[[6,6],[6,6]],[[6,0],[6,7]]],[[[7,7],[8,9]],[8,[8,1]]]]
//
// [[[[6,6],[6,6]],[[6,0],[6,7]]],[[[7,7],[8,9]],[8,[8,1]]]]
// + [2,9]
// = [[[[6,6],[7,7]],[[0,7],[7,7]]],[[[5,5],[5,6]],9]]
//
// [[[[6,6],[7,7]],[[0,7],[7,7]]],[[[5,5],[5,6]],9]]
// + [1,[[[9,3],9],[[9,0],[0,7]]]]
// = [[[[7,8],[6,7]],[[6,8],[0,8]]],[[[7,7],[5,0]],[[5,5],[5,6]]]]
//
// [[[[7,8],[6,7]],[[6,8],[0,8]]],[[[7,7],[5,0]],[[5,5],[5,6]]]]
// + [[[5,[7,4]],7],1]
// = [[[[7,7],[7,7]],[[8,7],[8,7]]],[[[7,0],[7,7]],9]]
//
// [[[[7,7],[7,7]],[[8,7],[8,7]]],[[[7,0],[7,7]],9]]
// + [[[[4,2],2],6],[8,7]]
// = [[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]
//
// To check whether it's the right answer, the snailfish teacher only
// checks the magnitude of the final sum. The magnitude of a pair is 3
// times the magnitude of its left element plus 2 times the magnitude of
// its right element. The magnitude of a regular number is just that
// number.
//
// For example, the magnitude of [9,1] is 3*9 + 2*1 = 29; the magnitude
// of [1,9] is 3*1 + 2*9 = 21. Magnitude calculations are recursive: the
// magnitude of [[9,1],[1,9]] is 3*29 + 2*21 = 129.
//
// Here are a few more magnitude examples:
//
// * [[1,2],[[3,4],5]] becomes 143.
//
// * [[[[0,7],4],[[7,8],[6,0]]],[8,1]] becomes 1384.
//
// * [[[[1,1],[2,2]],[3,3]],[4,4]] becomes 445.
//
// * [[[[3,0],[5,3]],[4,4]],[5,5]] becomes 791.
//
// * [[[[5,0],[7,4]],[5,5]],[6,6]] becomes 1137.
//
// * [[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]] becomes 3488.
//
// So, given this example homework assignment:
//
// [[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
// [[[5,[2,8]],4],[5,[[9,9],0]]]
// [6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
// [[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
// [[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
// [[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
// [[[[5,4],[7,7]],8],[[8,3],8]]
// [[9,3],[[9,9],[6,[4,9]]]]
// [[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
// [[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]
//
// The final sum is:
//
// [[[[6,6],[7,6]],[[7,7],[7,0]]],[[[7,7],[7,7]],[[7,8],[9,9]]]]
//
// The magnitude of this final sum is 4140.
//
// Add up all of the snailfish numbers from the homework assignment in
// the order they appear. What is the magnitude of the final sum?
//
// --- Part Two ---
// ----------------
//
// You notice a second question on the back of the homework assignment:
//
// What is the largest magnitude you can get from adding only two of the
// snailfish numbers?
//
// Note that snailfish addition is not commutative - that is, x + y and y
// + x can produce different results.
//
// Again considering the last example homework assignment above:
//
// [[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]
// [[[5,[2,8]],4],[5,[[9,9],0]]]
// [6,[[[6,2],[5,6]],[[7,6],[4,7]]]]
// [[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]
// [[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]
// [[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]
// [[[[5,4],[7,7]],8],[[8,3],8]]
// [[9,3],[[9,9],[6,[4,9]]]]
// [[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]
// [[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]
//
// The largest magnitude of the sum of any two snailfish numbers in this
// list is 3993. This is the magnitude of
// [[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]] +
// [[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]], which reduces to
// [[[[7,8],[6,6]],[[6,0],[7,7]]],[[[7,8],[8,8]],[[7,9],[0,6]]]].
//
// What is the largest magnitude of any sum of two different snailfish
// numbers from the homework assignment?

#include "advent_of_code/2021/day18/day18.h"

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

class SnailFishStack {
 public:
  friend std::ostream& operator<<(std::ostream& o, const SnailFishStack& s) {
    int depth = 0;
    std::vector<bool> print_left;
    for (const auto& r : s.values) {
      if (!print_left.empty() && print_left.back()) {
        o << ",";
      }
      while (depth < r.depth) {
        ++depth;
        o << "[";
        print_left.push_back(false);
      }
      o << r.value;
      while (!print_left.empty() && print_left.back()) {
        o << "]";
        print_left.pop_back();
        --depth;
      }
      if (!print_left.empty()) {
        print_left.back() = true;
      }
    }
    while (!print_left.empty() && print_left.back()) {
      o << "]";
      print_left.pop_back();
    }
    return o;
  }

  static absl::StatusOr<SnailFishStack> Parse(absl::string_view& line) {
    int depth = 0;
    SnailFishStack ret;
    for (int i = 0; i < line.size(); ++i) {
      if (line[i] == '[') {
        ++depth;
        continue;
      } else if (line[i] == ',') {
        continue;
      } else if (line[i] == ']') {
        --depth;
        continue;
      } else if (line[i] >= '0' && line[i] <= '9') {
        int value = 0;
        while (line[i] >= '0' && line[i] <= '9') {
          value = value * 10 + line[i] - '0';
          ++i;
        }
        --i;
        ret.values.push_back({.depth = depth, .value = value});
      } else {
        return Error("Bad line");
      }
    }
    return ret;
  }
  SnailFishStack Add(const SnailFishStack& other) const {
    SnailFishStack ret;
    for (const auto& v : values) {
      ret.values.push_back({.depth = v.depth + 1, .value = v.value});
    }
    for (const auto& v : other.values) {
      ret.values.push_back({.depth = v.depth + 1, .value = v.value});
    }
    VLOG(2) << "Add";
    VLOG(2) << ret;
    ret.Reduce();
    return ret;
  }
  int64_t Magnitude() const {
    std::vector<bool> on_left;
    std::vector<int> stack;
    VLOG(3) << "Magnitude: " << *this;
    for (const auto& r : values) {
      while (on_left.size() < r.depth) {
        on_left.push_back(false);
      }
      int value = r.value;
      if (!on_left.back()) {
        VLOG(3) << "push: " << value;
        stack.push_back(value);
        on_left.back() = true;
      } else {
        while (!on_left.empty() && on_left.back()) {
          int new_val = stack.back() * 3 + 2 * value;
          VLOG(3) << "3 * " << stack.back() << " + 2 * " << value << " = "
                  << new_val;
          stack.pop_back();
          value = new_val;
          on_left.pop_back();
        }
        stack.push_back(value);
        if (!on_left.empty()) {
          on_left.back() = true;
        }
      }
    }
    CHECK_EQ(stack.size(), 1);
    return stack[0];
  }
  void Reduce() {
    bool work_done = true;
    while (work_done) {
      VLOG(2) << "Reduce";
      VLOG(2) << *this;
      work_done = false;
      work_done = TryExplode();
      if (work_done) continue;
      work_done = TrySplit();
    }
  }
  bool TryExplode() {
    for (int i = 0; i < values.size() - 1; ++i) {
      if (values[i].depth > 4) {
        if (i > 0) values[i - 1].value += values[i].value;
        CHECK(values[i + 1].depth > 4);
        if (i + 2 < values.size()) {
          values[i + 2].value += values[i + 1].value;
        }
        values[i].value = 0;
        --values[i].depth;
        for (int j = i + 1; j < values.size() - 1; ++j) {
          values[j] = values[j + 1];
        }
        values.resize(values.size() - 1);
        return true;
      }
    }
    CHECK_LE(values.back().depth, 4);
    return false;
  }
  bool TrySplit() {
    for (int i = 0; i < values.size(); i++) {
      if (values[i].value >= 10) {
        values.resize(values.size() + 1);
        for (int j = values.size() - 1; j > i + 1; --j) {
          values[j] = values[j - 1];
        }
        values[i + 1].value = (values[i].value + 1) / 2;
        values[i + 1].depth = values[i].depth + 1;
        values[i].value = values[i].value / 2;
        values[i].depth = values[i].depth + 1;
        return true;
      }
    }
    return false;
  }

 private:
  struct Rec {
    int depth;
    int value;
  };
  std::vector<Rec> values;
};

class SnailFishTree {
 public:
  friend std::ostream& operator<<(std::ostream& o, const SnailFishTree& s) {
    if (s.value) return o << *s.value;
    CHECK(s.left->parent == &s);
    CHECK(s.right->parent == &s);
    return o << "[" << *s.left << "," << *s.right << "]";
  }

  static absl::StatusOr<std::unique_ptr<SnailFishTree>> Parse(
      absl::string_view line) {
    absl::StatusOr<std::unique_ptr<SnailFishTree>> rec = ParseImpl(line);
    if (!rec.ok()) return rec.status();
    if (!line.empty()) return absl::InvalidArgumentError("No full parse");
    return rec;
  }

  static absl::StatusOr<std::unique_ptr<SnailFishTree>> ParseImpl(
      absl::string_view& line) {
    std::unique_ptr<SnailFishTree> ret = absl::make_unique<SnailFishTree>();
    if (line.empty()) return Error("Empty");
    if (line[0] == '[') {
      line = line.substr(1);

      absl::StatusOr<std::unique_ptr<SnailFishTree>> left = ParseImpl(line);
      if (!left.ok()) return left.status();
      ret->left = std::move(*left);
      ret->left->parent = ret.get();

      if (line[0] != ',') return Error("Bad parse ','");
      line = line.substr(1);

      absl::StatusOr<std::unique_ptr<SnailFishTree>> right = ParseImpl(line);
      if (!right.ok()) return right.status();
      ret->right = std::move(*right);
      ret->right->parent = ret.get();

      if (line[0] != ']') return Error("Bad parse ']'");
      line = line.substr(1);
      return ret;
    }
    if (line[0] < '0' || line[0] > '9') return Error("Bad parse: digit");
    ret->value = 0;
    while (!line.empty() && line[0] >= '0' && line[0] <= '9') {
      ret->value = *ret->value * 10 + line[0] - '0';
      line = line.substr(1);
    }
    return ret;
  }
  std::unique_ptr<SnailFishTree> Clone() const {
    std::unique_ptr<SnailFishTree> ret = absl::make_unique<SnailFishTree>();
    if (value) {
      ret->value = value;
      return ret;
    }
    ret->left = left->Clone();
    ret->left->parent = ret.get();
    ret->right = right->Clone();
    ret->right->parent = ret.get();
    return ret;
  }
  std::unique_ptr<SnailFishTree> Add(
      const std::unique_ptr<SnailFishTree>& other) const {
    std::unique_ptr<SnailFishTree> ret = absl::make_unique<SnailFishTree>();
    ret->left = Clone();
    ret->left->parent = ret.get();
    ret->right = other->Clone();
    ret->right->parent = ret.get();
    ret->Reduce();
    return ret;
  }
  int64_t Magnitude() const {
    if (value) return *value;
    return 3 * left->Magnitude() + 2 * right->Magnitude();
  }
  void Reduce() {
    bool work_done = true;
    while (work_done) {
      VLOG(2) << "Reduce";
      VLOG(2) << *this;
      work_done = false;
      work_done = TryExplode(0);
      if (work_done) continue;
      work_done = TrySplit();
    }
  }
  SnailFishTree* NearestLeft(SnailFishTree* from) {
    if (from == right.get()) return left->RightMost();
    CHECK(from == left.get());
    if (parent == nullptr) return nullptr;
    return parent->NearestLeft(this);
  }
  SnailFishTree* NearestRight(SnailFishTree* from) {
    if (from == left.get()) return right->LeftMost();
    CHECK(from == right.get());
    if (parent == nullptr) return nullptr;
    return parent->NearestRight(this);
  }
  SnailFishTree* LeftMost() {
    if (value) return this;
    return left->LeftMost();
  }
  SnailFishTree* RightMost() {
    if (value) return this;
    return right->RightMost();
  }
  bool TryExplode(int depth) {
    if (value) return false;
    if (depth == 4) {
      SnailFishTree* nearest_left = parent->NearestLeft(this);
      if (nearest_left) {
        CHECK(left->value);
        CHECK(nearest_left->value);
        nearest_left->value = *nearest_left->value + *left->value;
      }
      SnailFishTree* nearest_right = parent->NearestRight(this);
      if (nearest_right) {
        CHECK(right->value);
        CHECK(nearest_right->value);
        nearest_right->value = *nearest_right->value + *right->value;
      }
      value = 0;
      left.reset();
      right.reset();
      return true;
    }
    if (left->TryExplode(depth + 1)) return true;
    return right->TryExplode(depth + 1);
  }
  bool TrySplit() {
    if (value) {
      if (*value >= 10) {
        left = absl::make_unique<SnailFishTree>();
        left->value = *value / 2;
        left->parent = this;
        right = absl::make_unique<SnailFishTree>();
        right->value = *value - *left->value;
        right->parent = this;
        value = {};
        return true;
      }
      return false;
    }
    if (left->TrySplit()) return true;
    return right->TrySplit();
  }

 private:
  absl::optional<int> value;
  std::unique_ptr<SnailFishTree> left;
  std::unique_ptr<SnailFishTree> right;
  SnailFishTree* parent = nullptr;
};

absl::Status Audit(absl::Span<absl::string_view> input) {
  absl::optional<SnailFishStack> total_stack;
  std::unique_ptr<SnailFishTree> total_tree;

  for (absl::string_view in_str : input) {
    absl::StatusOr<SnailFishStack> in_stack = SnailFishStack::Parse(in_str);
    if (!in_stack.ok()) return in_stack.status();
    if (!total_stack) {
      total_stack = std::move(*in_stack);
    } else {
      total_stack = total_stack->Add(std::move(*in_stack));
    }

    absl::StatusOr<std::unique_ptr<SnailFishTree>> in_tree =
        SnailFishTree::Parse(in_str);
    if (!in_tree.ok()) return in_tree.status();
    if (!total_tree) {
      total_tree = std::move(*in_tree);
    } else {
      total_tree = total_tree->Add(std::move(*in_tree));
    }
  }

  if (total_stack->Magnitude() != total_tree->Magnitude()) {
    return Error("Stack and tree differ");
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_18::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    if (auto st = Audit(input); !st.ok()) return st;
    VLOG(1) << "Audit: OK";
  }

  absl::optional<SnailFishStack> total;
  for (absl::string_view in_str : input) {
    absl::StatusOr<SnailFishStack> in = SnailFishStack::Parse(in_str);
    if (!in.ok()) return in.status();
    VLOG(2) << in_str;
    VLOG(2) << *in;
    if (!total) {
      total = std::move(*in);
    } else {
      total = total->Add(std::move(*in));
    }
    VLOG(2) << "Total: " << *total;
  }
  if (!total) return absl::NotFoundError("No snailfish record");
  return IntReturn(total->Magnitude());
}

absl::StatusOr<std::string> Day_2021_18::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<SnailFishStack> values;
  for (absl::string_view in_str : input) {
    absl::StatusOr<SnailFishStack> in = SnailFishStack::Parse(in_str);
    if (!in.ok()) return in.status();
    values.push_back(std::move(*in));
  }
  int max = 0;
  for (const auto& sf1 : values) {
    for (const auto& sf2 : values) {
      int test = sf1.Add(sf2).Magnitude();
      max = std::max(max, test);
    }
  }
  if (values.empty()) return absl::NotFoundError("No snailfish record");
  return IntReturn(max);
}

}  // namespace advent_of_code
