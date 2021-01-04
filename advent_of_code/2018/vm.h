#ifndef ADVENT_OF_CODE_2018_VM_H
#define ADVENT_OF_CODE_2018_VM_H

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

enum OpCode {
  kAddr = 1,
  kAddi = 2,
  kMulr = 3,
  kMuli = 4,
  kBanr = 5,
  kBani = 6,
  kBorr = 7,
  kBori = 8,
  kSetr = 9,
  kSeti = 10,
  kGtir = 11,
  kGtri = 12,
  kGtrr = 13,
  kEqir = 14,
  kEqri = 15,
  kEqrr = 16,
};

struct Op {
  static const absl::flat_hash_map<absl::string_view, OpCode> kNameMap;
  static const std::vector<absl::string_view> kCodeMap;

  OpCode op_code;
  int32_t arg1;
  int32_t arg2;
  int32_t arg3;

  friend std::ostream& operator<<(std::ostream& out, const Op& op) {
    return out << kCodeMap[op.op_code] << ": " << op.arg1 << ", " << op.arg2 << ", " << op.arg3;
  }

  static absl::StatusOr<Op> Parse(absl::string_view str) {
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

  absl::Status Apply(std::vector<int32_t>& registers) {
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
        return absl::InvalidArgumentError(
            absl::StrCat("Bad op_code: ", op_code));
    }
    return absl::OkStatus();
  }
};

// static
const absl::flat_hash_map<absl::string_view, OpCode> Op::kNameMap = {
    {"addr", OpCode::kAddr}, {"addi", OpCode::kAddi}, {"mulr", OpCode::kMulr},
    {"muli", OpCode::kMuli}, {"banr", OpCode::kBanr}, {"bani", OpCode::kBani},
    {"borr", OpCode::kBorr}, {"bori", OpCode::kBori}, {"setr", OpCode::kSetr},
    {"seti", OpCode::kSeti}, {"gtir", OpCode::kGtir}, {"gtri", OpCode::kGtri},
    {"gtrr", OpCode::kGtrr}, {"eqrr", OpCode::kEqrr}, {"eqir", OpCode::kEqir},
    {"eqri", OpCode::kEqri},
};

const std::vector<absl::string_view> Op::kCodeMap = {
  "<NONE>",
  "kAddr",
  "kAddi",
  "kMulr",
  "kMuli",
  "kBanr",
  "kBani",
  "kBorr",
  "kBori",
  "kSetr",
  "kSeti",
  "kGtir",
  "kGtri",
  "kGtrr",
  "kEqir",
  "kEqri",
  "kEqrr"};

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
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

  absl::Status Execute() {
    int32_t* ip = &registers_[ip_register_];
    while (*ip >= 0 && *ip < ops_.size()) {
      VLOG(4) << *ip << ": " << absl::StrJoin(registers_, ",");
      if (absl::Status st = ops_[*ip].Apply(registers_); !st.ok()) {
        return st;
      }
      ++*ip;
    }
    return absl::OkStatus();
  }

  absl::Status ExecuteAndWatch(std::function<bool(int)> watcher) {
    int32_t* ip = &registers_[ip_register_];
    while (*ip >= 0 && *ip < ops_.size()) {
      if (watcher(*ip)) break;
      VLOG(4) << *ip << ": " << absl::StrJoin(registers_, ",");
      if (absl::Status st = ops_[*ip].Apply(registers_); !st.ok()) {
        return st;
      }
      ++*ip;
    }
    return absl::OkStatus();
  }

  int32_t register_value(int register_number) const {
    return registers_[register_number];
  }

  void set_register_value(int register_number, int32_t value) {
    registers_[register_number] = value;
  }

  const std::vector<Op>& ops() const { return ops_; }

 private:
  VM() : registers_(6, 0) {}

  std::vector<Op> ops_;
  int ip_register_;
  std::vector<int32_t> registers_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2018_VM_H