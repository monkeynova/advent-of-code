#include "advent_of_code/2020/day8/day8.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2020/boot_code.h"
#include "glog/logging.h"
#include "re2/re2.h"

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
