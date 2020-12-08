#include "advent_of_code/2020/day8/day8.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

class Code {
 private:
  enum class Instruction {
    kJmp = 0,
    kAcc = 1,
    kNop = 2
  };
  struct Statement {
    Instruction i;
    int64_t arg;
  };

 public:
  static absl::StatusOr<Code> Parse(const std::vector<absl::string_view>& input) {
    absl::flat_hash_map<absl::string_view, Instruction> imap = {
      {"jmp", Instruction::kJmp},
      {"acc", Instruction::kAcc},
      {"nop", Instruction::kNop}
    };
    std::vector<Statement> statements;
    for (absl::string_view str : input) {
      absl::string_view cmd;
      int arg;
      if (!RE2::FullMatch(str, "([a-z]*) ([\\-+]?\\d+)", &cmd, &arg)) {
        return absl::InvalidArgumentError(absl::StrCat("re: ", str));
      }
      auto it = imap.find(cmd);
      if (it == imap.end()) return absl::InvalidArgumentError("cmd");
      statements.push_back(Statement{it->second, arg});
    }
    return Code(std::move(statements));
  }

  absl::Status Execute() {
    absl::flat_hash_set<int> hist;
    int ip = 0;
    while (!hist.contains(ip)) {
      hist.insert(ip);
      switch (statements_[ip].i) {
        case Instruction::kJmp: ip += statements_[ip].arg; break;
        case Instruction::kAcc: accumulator_ += statements_[ip].arg; ++ip; break;
        case Instruction::kNop: ++ip; break;
        default: return absl::InternalError("bad instruction");
      }
    }
    return absl::OkStatus();
  }

  int accumulator() { return accumulator_; }

 private:
  Code(std::vector<Statement> statements) : statements_(std::move(statements)) {}
  std::vector<Statement> statements_;
  int accumulator_ = 0;
};

absl::StatusOr<std::vector<std::string>> Day8_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<Code> codes = Code::Parse(input);
  if (!codes.ok()) return codes.status();

  if (absl::Status st = codes->Execute(); !st.ok()) return st;
  return IntReturn(codes->accumulator());
}

absl::StatusOr<std::vector<std::string>> Day8_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}
