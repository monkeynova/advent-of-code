#include "advent_of_code/2020/day8/day8.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

class BootCode {
 public:
  enum class Instruction { kJmp = 0, kAcc = 1, kNop = 2 };
  struct Statement {
    Instruction i;
    int64_t arg;
  };

  BootCode(std::vector<Statement> statements)
      : statements_(std::move(statements)) {}

  static absl::StatusOr<BootCode> Parse(
      const std::vector<absl::string_view>& input) {
    absl::flat_hash_map<absl::string_view, Instruction> imap = {
        {"jmp", Instruction::kJmp},
        {"acc", Instruction::kAcc},
        {"nop", Instruction::kNop}};
    std::vector<Statement> statements;
    for (absl::string_view str : input) {
      absl::string_view cmd;
      int arg;
      static LazyRE2 statement_re{"([a-z]*) ([\\-+]?\\d+)"}; 
      if (!RE2::FullMatch(str, *statement_re, &cmd, &arg)) {
        return absl::InvalidArgumentError(absl::StrCat("re: ", str));
      }
      auto it = imap.find(cmd);
      if (it == imap.end()) return absl::InvalidArgumentError("cmd");
      statements.push_back(Statement{it->second, arg});
    }
    return BootCode(std::move(statements));
  }

  absl::StatusOr<bool> Execute() {
    absl::flat_hash_set<int> hist;
    int ip = 0;
    while (!hist.contains(ip)) {
      hist.insert(ip);
      if (ip >= statements_.size()) return true;
      switch (statements_[ip].i) {
        case Instruction::kJmp:
          ip += statements_[ip].arg;
          break;
        case Instruction::kAcc:
          accumulator_ += statements_[ip].arg;
          ++ip;
          break;
        case Instruction::kNop:
          ++ip;
          break;
        default:
          return absl::InternalError("bad instruction");
      }
    }
    return false;
  }

  int accumulator() { return accumulator_; }
  const std::vector<Statement>& statements() { return statements_; }

 private:
  std::vector<Statement> statements_;
  int accumulator_ = 0;
};

absl::StatusOr<std::vector<std::string>> Day8_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<BootCode> boot_code = BootCode::Parse(input);
  if (!boot_code.ok()) return boot_code.status();

  absl::StatusOr<bool> terminated = boot_code->Execute();
  if (!terminated.ok()) return terminated.status();

  return IntReturn(boot_code->accumulator());
}

absl::StatusOr<std::vector<std::string>> Day8_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<BootCode> boot_code = BootCode::Parse(input);
  if (!boot_code.ok()) return boot_code.status();

  for (int i = 0; i < boot_code->statements().size(); ++i) {
    std::vector<BootCode::Statement> edit_statements = boot_code->statements();
    if (edit_statements[i].i == BootCode::Instruction::kJmp) {
      edit_statements[i].i = BootCode::Instruction::kNop;
    } else if (edit_statements[i].i == BootCode::Instruction::kNop) {
      edit_statements[i].i = BootCode::Instruction::kJmp;
    } else {
      continue;
    }
    BootCode tmp_code(std::move(edit_statements));
    absl::StatusOr<bool> terminated = tmp_code.Execute();
    if (!terminated.ok()) return terminated.status();
    if (*terminated) {
      return IntReturn(tmp_code.accumulator());
    }
  }

  return absl::InvalidArgumentError("No version terminated");
}
