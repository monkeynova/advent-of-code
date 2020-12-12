#ifndef ADVENT_OF_CODE_2020_BOOT_CODE_H
#define ADVENT_OF_CODE_2020_BOOT_CODE_H

#include <vector>

#include "absl/status/statusor.h"

class BootCode {
 public:
  enum class Instruction { kJmp = 0, kAcc = 1, kNop = 2 };
  struct Statement {
    Instruction i;
    int64_t arg;
  };

  BootCode(std::vector<Statement> statements)
      : statements_(std::move(statements)) {}

  static absl::StatusOr<BootCode> Parse(absl::Span<absl::string_view> input);

  absl::StatusOr<bool> Execute();

  int accumulator() { return accumulator_; }
  const std::vector<Statement>& statements() { return statements_; }

 private:
  std::vector<Statement> statements_;
  int accumulator_ = 0;
};

#endif  // ADVENT_OF_CODE_2020_BOOT_CODE_H