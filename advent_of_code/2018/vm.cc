#include "advent_of_code/2018/vm.h"

namespace advent_of_code {

// static
const absl::flat_hash_map<absl::string_view, VM::OpCode> VM::Op::kNameMap = {
    {"addr", OpCode::kAddr}, {"addi", OpCode::kAddi}, {"mulr", OpCode::kMulr},
    {"muli", OpCode::kMuli}, {"banr", OpCode::kBanr}, {"bani", OpCode::kBani},
    {"borr", OpCode::kBorr}, {"bori", OpCode::kBori}, {"setr", OpCode::kSetr},
    {"seti", OpCode::kSeti}, {"gtir", OpCode::kGtir}, {"gtri", OpCode::kGtri},
    {"gtrr", OpCode::kGtrr}, {"eqrr", OpCode::kEqrr}, {"eqir", OpCode::kEqir},
    {"eqri", OpCode::kEqri},
};

// static
const std::vector<absl::string_view> VM::Op::kCodeMap = {
    "<NONE>", "kAddr", "kAddi", "kMulr", "kMuli", "kBanr",
    "kBani",  "kBorr", "kBori", "kSetr", "kSeti", "kGtir",
    "kGtri",  "kGtrr", "kEqir", "kEqri", "kEqrr"};

// static
absl::StatusOr<VM::Op> VM::Op::Parse(absl::string_view str) {
  Op op;
  absl::string_view op_name;
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

absl::Status VM::Op::Apply(std::vector<int32_t>& registers) const {
  switch (op_code) {
    case OpCode::kAddr: {
      registers[arg3] = registers[arg1] + registers[arg2];
      break;
    }
    case OpCode::kAddi: {
      registers[arg3] = registers[arg1] + arg2;
      break;
    }
    case OpCode::kMulr: {
      registers[arg3] = registers[arg1] * registers[arg2];
      break;
    }
    case OpCode::kMuli: {
      registers[arg3] = registers[arg1] * arg2;
      break;
    }
    case OpCode::kBanr: {
      registers[arg3] = registers[arg1] & registers[arg2];
      break;
    }
    case OpCode::kBani: {
      registers[arg3] = registers[arg1] & arg2;
      break;
    }
    case OpCode::kBorr: {
      registers[arg3] = registers[arg1] | registers[arg2];
      break;
    }
    case OpCode::kBori: {
      registers[arg3] = registers[arg1] | arg2;
      break;
    }
    case OpCode::kSetr: {
      registers[arg3] = registers[arg1];
      break;
    }
    case OpCode::kSeti: {
      registers[arg3] = arg1;
      break;
    }
    case OpCode::kGtir: {
      registers[arg3] = arg1 > registers[arg2] ? 1 : 0;
      break;
    }
    case OpCode::kGtri: {
      registers[arg3] = registers[arg1] > arg2 ? 1 : 0;
      break;
    }
    case OpCode::kGtrr: {
      registers[arg3] = registers[arg1] > registers[arg2] ? 1 : 0;
      break;
    }
    case OpCode::kEqir: {
      registers[arg3] = arg1 == registers[arg2] ? 1 : 0;
      break;
    }
    case OpCode::kEqri: {
      registers[arg3] = registers[arg1] == arg2 ? 1 : 0;
      break;
    }
    case OpCode::kEqrr: {
      registers[arg3] = registers[arg1] == registers[arg2] ? 1 : 0;
      break;
    }
    default:
      return absl::InvalidArgumentError(absl::StrCat("Bad op_code: ", op_code));
  }
  return absl::OkStatus();
}

// static
absl::StatusOr<VM> VM::Parse(absl::Span<absl::string_view> input) {
  VM vm;
  for (absl::string_view row : input) {
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