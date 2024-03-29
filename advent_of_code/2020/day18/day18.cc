#include "advent_of_code/2020/day18/day18.h"

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

class ExprTree {
 public:
  virtual ~ExprTree() = default;
  virtual int64_t Eval() = 0;
  virtual std::string DebugString() = 0;
};

class Val : public ExprTree {
 public:
  explicit Val(int64_t val) : val_(val) {}

  int64_t Eval() override { return val_; }
  std::string DebugString() override { return absl::StrCat(val_); }

 private:
  int64_t val_;
};

class Mult : public ExprTree {
 public:
  Mult(std::unique_ptr<ExprTree> left, std::unique_ptr<ExprTree> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  int64_t Eval() override { return left_->Eval() * right_->Eval(); }
  std::string DebugString() override {
    return absl::StrCat("(", left_->DebugString(), "*", right_->DebugString(),
                        ")");
  }

 private:
  std::unique_ptr<ExprTree> left_;
  std::unique_ptr<ExprTree> right_;
};

class Add : public ExprTree {
 public:
  Add(std::unique_ptr<ExprTree> left, std::unique_ptr<ExprTree> right)
      : left_(std::move(left)), right_(std::move(right)) {}

  int64_t Eval() override { return left_->Eval() + right_->Eval(); }
  std::string DebugString() override {
    return absl::StrCat("(", left_->DebugString(), "+", right_->DebugString(),
                        ")");
  }

 private:
  std::unique_ptr<ExprTree> left_;
  std::unique_ptr<ExprTree> right_;
};

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree(std::string_view* str);

absl::StatusOr<std::unique_ptr<ExprTree>> ParseValue(std::string_view* str) {
  if (str->empty()) return Error("No value");
  while ((*str)[0] == ' ') *str = str->substr(1);
  if ((*str)[0] == '(') {
    *str = str->substr(1);
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> ret, ParseOpTree(str));
    if ((*str)[0] != ')') return Error("No matching ')'");
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
  return Error("can't get value from: ", *str);
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree(std::string_view* str) {
  ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> last_val, ParseValue(str));
  while (!str->empty()) {
    char op = (*str)[0];
    if (op == ')') break;
    *str = str->substr(1);
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> next_val, ParseValue(str));
    switch (op) {
      case '+': {
        last_val =
            absl::make_unique<Add>(std::move(last_val), std::move(next_val));
        break;
      }
      case '*': {
        last_val =
            absl::make_unique<Mult>(std::move(last_val), std::move(next_val));
        break;
      }
      default:
        return Error("Bad op: ", *str);
    }
  }
  return last_val;
}

absl::StatusOr<std::unique_ptr<ExprTree>> Parse(std::string_view str) {
  absl::StatusOr<std::unique_ptr<ExprTree>> ret = ParseOpTree(&str);
  if (!str.empty()) return Error("Bad parse remain=", str);
  return ret;
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree2(std::string_view* str);

absl::StatusOr<std::unique_ptr<ExprTree>> ParseValue2(std::string_view* str) {
  if (str->empty()) return Error("No value");
  while ((*str)[0] == ' ') *str = str->substr(1);
  if ((*str)[0] == '(') {
    *str = str->substr(1);
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> ret, ParseOpTree2(str));
    if ((*str)[0] != ')') return Error("No matching ')'");
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
  return Error("can't get value from: ", *str);
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseAddTree(std::string_view* str) {
  ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> last_val, ParseValue2(str));
  while (!str->empty()) {
    char op = (*str)[0];
    if (op == ')' || op == '*') break;
    if (op != '+') return Error("Not add: ", *str);
    *str = str->substr(1);
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> next_val, ParseValue2(str));
    last_val = absl::make_unique<Add>(std::move(last_val), std::move(next_val));
  }
  return last_val;
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseMulTree(std::string_view* str) {
  ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> last_val, ParseAddTree(str));
  while (!str->empty()) {
    char op = (*str)[0];
    if (op == ')') break;
    if (op != '*') return Error("Not mult: ", *str);
    *str = str->substr(1);
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> next_val, ParseAddTree(str));
    last_val =
        absl::make_unique<Mult>(std::move(last_val), std::move(next_val));
  }
  return last_val;
}

absl::StatusOr<std::unique_ptr<ExprTree>> ParseOpTree2(std::string_view* str) {
  return ParseMulTree(str);
}

absl::StatusOr<std::unique_ptr<ExprTree>> Parse2(std::string_view str) {
  absl::StatusOr<std::unique_ptr<ExprTree>> ret = ParseOpTree2(&str);
  if (!str.empty()) return Error("Bad parse remain=", str);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_18::Part1(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view expr : input) {
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> tree, Parse(expr));
    sum += tree->Eval();
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2020_18::Part2(
    absl::Span<std::string_view> input) const {
  int64_t sum = 0;
  for (std::string_view expr : input) {
    ASSIGN_OR_RETURN(std::unique_ptr<ExprTree> tree, Parse2(expr));
    VLOG(1) << tree->DebugString();
    sum += tree->Eval();
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/18,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_18()); });

}  // namespace advent_of_code
