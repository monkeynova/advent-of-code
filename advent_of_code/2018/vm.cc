#include "advent_of_code/2018/vm.h"

namespace advent_of_code {

// static
const absl::flat_hash_map<std::string_view, VM::OpCode> VM::Op::kNameMap = {
    {"addr", OpCode::kAddr}, {"addi", OpCode::kAddi}, {"mulr", OpCode::kMulr},
    {"muli", OpCode::kMuli}, {"banr", OpCode::kBanr}, {"bani", OpCode::kBani},
    {"borr", OpCode::kBorr}, {"bori", OpCode::kBori}, {"setr", OpCode::kSetr},
    {"seti", OpCode::kSeti}, {"gtir", OpCode::kGtir}, {"gtri", OpCode::kGtri},
    {"gtrr", OpCode::kGtrr}, {"eqrr", OpCode::kEqrr}, {"eqir", OpCode::kEqir},
    {"eqri", OpCode::kEqri},
};

// static
const std::vector<std::string_view> VM::Op::kCodeMap = {
    "<NONE>", "kAddr", "kAddi", "kMulr", "kMuli", "kBanr",
    "kBani",  "kBorr", "kBori", "kSetr", "kSeti", "kGtir",
    "kGtri",  "kGtrr", "kEqir", "kEqri", "kEqrr"};

// static
absl::StatusOr<VM::Op> VM::Op::Parse(std::string_view str) {
  Op op;
  std::string_view op_name;
  if (!RE2::FullMatch(str, "([a-z]+) (-?\\d+) (-?\\d+) (-?\\d+)", &op_name,
                      &op.arg1, &op.arg2, &op.arg3)) {
    return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
  }
  auto it = kNameMap.find(op_name);
  if (it == kNameMap.end()) {
    return absl::InvalidArgumentError(absl::StrCat("Bad op: ", op_name));
  }
  op.op_code = it->second;
  return op;
}

// static
absl::StatusOr<VM> VM::Parse(absl::Span<std::string_view> input) {
  VM vm;
  for (std::string_view row : input) {
    if (RE2::FullMatch(row, "#ip (\\d+)", &vm.ip_register_)) {
      if (vm.ip_register_ > vm.registers_.size()) {
        return absl::InvalidArgumentError(
            absl::StrCat("Bad register: ", vm.ip_register_));
      }
    } else {
      absl::StatusOr<Op> op = Op::Parse(row);
      if (!op.ok()) return op.status();
      vm.ops_.push_back(*op);
    }
  }
  return vm;
}

}  // namespace advent_of_code