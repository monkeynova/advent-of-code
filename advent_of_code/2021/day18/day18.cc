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

  static absl::StatusOr<SnailFishStack> Parse(std::string_view& line) {
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
      std::string_view line) {
    ASSIGN_OR_RETURN(std::unique_ptr<SnailFishTree> rec, ParseImpl(line));
    if (!line.empty()) return absl::InvalidArgumentError("No full parse");
    return rec;
  }

  static absl::StatusOr<std::unique_ptr<SnailFishTree>> ParseImpl(
      std::string_view& line) {
    std::unique_ptr<SnailFishTree> ret = absl::make_unique<SnailFishTree>();
    if (line.empty()) return Error("Empty");
    if (line[0] == '[') {
      line = line.substr(1);

      ASSIGN_OR_RETURN(std::unique_ptr<SnailFishTree> left, ParseImpl(line));
      ret->left = std::move(left);
      ret->left->parent = ret.get();

      if (line[0] != ',') return Error("Bad parse ','");
      line = line.substr(1);

      ASSIGN_OR_RETURN(std::unique_ptr<SnailFishTree> right, ParseImpl(line));
      ret->right = std::move(right);
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
  std::optional<int> value;
  std::unique_ptr<SnailFishTree> left;
  std::unique_ptr<SnailFishTree> right;
  SnailFishTree* parent = nullptr;
};

absl::Status Audit(absl::Span<std::string_view> input) {
  std::optional<SnailFishStack> total_stack;
  std::unique_ptr<SnailFishTree> total_tree;

  for (std::string_view in_str : input) {
    ASSIGN_OR_RETURN(SnailFishStack in_stack, SnailFishStack::Parse(in_str));
    if (!total_stack) {
      total_stack = std::move(in_stack);
    } else {
      total_stack = total_stack->Add(std::move(in_stack));
    }

    ASSIGN_OR_RETURN(
        std::unique_ptr<SnailFishTree> in_tree,
        SnailFishTree::Parse(in_str));
    if (!total_tree) {
      total_tree = std::move(in_tree);
    } else {
      total_tree = total_tree->Add(std::move(in_tree));
    }
  }

  if (total_stack->Magnitude() != total_tree->Magnitude()) {
    return Error("Stack and tree differ");
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_18::Part1(
    absl::Span<std::string_view> input) const {
  if (run_audit()) {
    if (auto st = Audit(input); !st.ok()) return st;
    VLOG(1) << "Audit: OK";
  }

  std::optional<SnailFishStack> total;
  for (std::string_view in_str : input) {
    ASSIGN_OR_RETURN(SnailFishStack in, SnailFishStack::Parse(in_str));
    VLOG(2) << in_str;
    VLOG(2) << in;
    if (!total) {
      total = std::move(in);
    } else {
      total = total->Add(std::move(in));
    }
    VLOG(2) << "Total: " << *total;
  }
  if (!total) return absl::NotFoundError("No snailfish record");
  return AdventReturn(total->Magnitude());
}

absl::StatusOr<std::string> Day_2021_18::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<SnailFishStack> values;
  for (std::string_view in_str : input) {
    ASSIGN_OR_RETURN(SnailFishStack in, SnailFishStack::Parse(in_str));
    values.push_back(std::move(in));
  }
  int max = 0;
  for (const auto& sf1 : values) {
    for (const auto& sf2 : values) {
      int test = sf1.Add(sf2).Magnitude();
      max = std::max(max, test);
    }
  }
  if (values.empty()) return absl::NotFoundError("No snailfish record");
  return AdventReturn(max);
}

}  // namespace advent_of_code
