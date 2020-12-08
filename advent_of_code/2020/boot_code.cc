#include "advent_of_code/2020/boot_code.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "re2/re2.h"

// static
absl::StatusOr<BootCode> BootCode::Parse(
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

absl::StatusOr<bool> BootCode::Execute() {
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
