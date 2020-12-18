#include "advent_of_code/2020/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

class ExprTree {
 public:
  virtual ~ExprTree() = default;
  virtual int64_t Eval() = 0;
};

class Val : public ExprTree {
 public:
  explicit Val(int64_t val) : val_(val) {}

  int64_t Eval() override { return val_; }

 private:
  int64_t val_;
};

class Mult : public ExprTree {
 public:
  Mult(std::unique_ptr<ExprTree> left, std::unique_ptr<ExprTree>right) : left_(std::move(left)), right_(std::move(right)) {}

  int64_t Eval() override { return left_->Eval() * right_->Eval(); }

 private:
  std::unique_ptr<ExprTree> left_;
  std::unique_ptr<ExprTree> right_;
};

class Add : public ExprTree {
 public:
  Add(std::unique_ptr<ExprTree> left, std::unique_ptr<ExprTree> right) : left_(std::move(left)), right_(std::move(right)) {}

  int64_t Eval() override { return left_->Eval() + right_->Eval(); }

 private:
  std::unique_ptr<ExprTree> left_;
  std::unique_ptr<ExprTree> right_;
};

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree(absl::string_view* str);

absl::StatusOr<std::unique_ptr<ExprTree>> ParseValue(absl::string_view* str) {
  if (str->empty()) return AdventDay::Error("No value");
  while ((*str)[0] == ' ') *str = str->substr(1);
  if ((*str)[0] == '(') {
    *str = str->substr(1);
    absl::StatusOr<std::unique_ptr<ExprTree>> ret = ParseOpTree(str);
    if (!ret.ok()) return ret.status();
    if ((*str)[0] != ')') return AdventDay::Error("No matching ')'");
    *str = str->substr(1);
    while ((*str)[0] == ' ') *str = str->substr(1);
    return ret;
  } else if ((*str)[0] >= '0' && (*str)[0] <= '9') {
    int64_t val = 0;
    while ((*str)[0] >= '0' && (*str)[0] <= '9') {
      val = val * 10 + (*str)[0] - '0';
      *str = str->substr(1);
    }
    while ((*str)[0] == ' ') *str = str->substr(1);
    return absl::make_unique<Val>(val);
  }
  return AdventDay::Error("can't get value from: ", *str);
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree(absl::string_view* str) {
  absl::StatusOr<std::unique_ptr<ExprTree>> last_val = ParseValue(str);
  if (!last_val.ok()) return last_val.status();
  while (!str->empty()) {
    char op = (*str)[0];
    if (op == ')') break;
    *str = str->substr(1);
    absl::StatusOr<std::unique_ptr<ExprTree>> next_val = ParseValue(str);
    if (!next_val.ok()) return next_val.status();
    switch (op) {
      case '+': {
        last_val = absl::make_unique<Add>(std::move(*last_val), std::move(*next_val));
        break;
      }
      case '*': {
        last_val = absl::make_unique<Mult>(std::move(*last_val), std::move(*next_val));
        break;
      }
      default: return AdventDay::Error("Bad op: ", *str);
    }
  }
  return last_val;
}

absl::StatusOr<std::unique_ptr<ExprTree>> Parse(absl::string_view str) {
  absl::StatusOr<std::unique_ptr<ExprTree>> ret = ParseOpTree(&str);
  if (!str.empty()) return AdventDay::Error("Bad parse remain=", str);
  return ret;
}

absl::StatusOr<std::vector<std::string>> Day18_2020::Part1(
    absl::Span<absl::string_view> input) const {
  int64_t sum = 0;
  for (absl::string_view expr : input) {
    absl::StatusOr<std::unique_ptr<ExprTree>> tree = Parse(expr);
    if (!tree.ok()) return tree.status();
    sum += (*tree)->Eval();
  }
  return IntReturn(sum);
}

absl::StatusOr<std::vector<std::string>> Day18_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
