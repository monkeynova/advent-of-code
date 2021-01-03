#include "advent_of_code/2018/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

/*
Scratch analysis of input.

#ip 5
00: addi 5 16 5  // JMP =16
01: seti 1 1 4   // [4] = 1
02: seti 1 8 2   // [2] = 1
03: mulr 4 2 3   // [4] * [2] -> [3]
04: eqrr 3 1 3   // [3] == [1] -> [3]
05: addr 3 5 5   // JMP +[3]
06: addi 5 1 5   // JMP +1
07: addr 4 0 0   // [0] += [4]
08: addi 2 1 2   // ++[2]
09: gtrr 2 1 3   // [2] > [1] -> [3]
10: addr 5 3 5   // JMP +[3]
11: seti 2 6 5   // JMP =2
12: addi 4 1 4   // ++[4]
13: gtrr 4 1 3   // [4] > [1] -> [3]
14: addr 3 5 5   // JMP +[3]
15: seti 1 4 5   // JMP =1
16: mulr 5 5 5   // TERM # JMP [5]*[5]
17: addi 1 2 1   // [1] += 2
18: mulr 1 1 1   // [1] *= [1]
19: mulr 5 1 1   // [1] *= [5]  # [5] == 19
20: muli 1 11 1  // [1] * = 11
21: addi 3 7 3   // [3] += 7
22: mulr 3 5 3   // [3] *= [5] # [5] = 22
23: addi 3 8 3   // [3] += 8
24: addr 1 3 1   // [1] += [3]
25: addr 5 0 5   // JMP +[0]
26: seti 0 9 5   // JMP = 0
27: setr 5 8 3   // [3] = [5] # 27
28: mulr 3 5 3   // [3] *= [5] # [3] = 27 * 28
29: addr 5 3 3   // [3] += [5] # [3] = 27 * 28 + 29
30: mulr 5 3 3   // [3] *= [5] # [3] = 30 * (27 * 28 + 29)
31: muli 3 14 3  // [3] *= 14 # [3] = 14 * 30 * (27 * 28 + 29)
32: mulr 3 5 3   // [3] *= [5] # [3] = 32 * 14 * 30 * (27 * 28 + 29)
33: addr 1 3 1   // [1] += [3]
34: seti 0 4 0   // [0] = 0
35: seti 0 3 5   // JMP =0

  a += 2;
  a *= a; // 4
  a *= 19;
  a *= 11;  // 836
  c += 7;
  c *= 22;
  c += 8;  // 162
  a += c;  // 998
  if (z == 1) {
    c = 10550400;
    a += c; // 10551398;
  }
    for (d = 1; d <= a; ++d) {
      for (b = 1; b <= a; ++b) {
        if (b * d == a) {
          z += d;
        }
      }
    }
*/

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

  OpCode op_code;
  int arg1;
  int arg2;
  int arg3;

  static absl::StatusOr<Op> Parse(absl::string_view str) {
    Op op;
    absl::string_view op_name;
    if (!RE2::FullMatch(str, "([a-z]+) (-?\\d+) (-?\\d+) (-?\\d+)", &op_name,
                        &op.arg1, &op.arg2, &op.arg3)) {
      return AdventDay::Error("Bad instruction: ", str);
    }
    auto it = kNameMap.find(op_name);
    if (it == kNameMap.end()) {
      return AdventDay::Error("Bad op: ", op_name);
    }
    op.op_code = it->second;
    return op;
  }

  absl::Status Apply(std::vector<int>& registers) {
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
        return AdventDay::Error("Bad op_code: ", op_code);
    }
    return absl::OkStatus();
  }
};

// static
const absl::flat_hash_map<absl::string_view, OpCode> Op::kNameMap = {
    {"addr", OpCode::kAddr}, {"addi", OpCode::kAddi}, {"mulr", OpCode::kMulr},
    {"muli", OpCode::kMuli}, {"banr", OpCode::kBanr}, {"bani", OpCode::kBani},
    {"borr", OpCode::kBorr}, {"bori", OpCode::kBorr}, {"setr", OpCode::kSetr},
    {"seti", OpCode::kSeti}, {"gtir", OpCode::kGtir}, {"gtri", OpCode::kGtri},
    {"gtrr", OpCode::kGtrr}, {"eqrr", OpCode::kEqrr}, {"eqir", OpCode::kEqir},
    {"eqri", OpCode::kEqri},
};

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM vm;
    for (absl::string_view row : input) {
      if (RE2::FullMatch(row, "#ip (\\d+)", &vm.ip_register_)) {
        if (vm.ip_register_ > vm.registers_.size()) {
          return AdventDay::Error("Bad register: ", vm.ip_register_);
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
    int* ip = &registers_[ip_register_];
    while (*ip >= 0 && *ip < ops_.size()) {
      if (*ip == 1 || *ip == 2) {
        LOG(INFO) << *ip << ": " << absl::StrJoin(registers_, ",");
      }
      if (absl::Status st = ops_[*ip].Apply(registers_); !st.ok()) {
        return st;
      }
      ++*ip;
    }
    return absl::OkStatus();
  }

  int register_value(int register_number) const {
    return registers_[register_number];
  }

  void set_register_value(int register_number, int value) {
    registers_[register_number] = value;
  }

 private:
  VM() : registers_(6, 0) {}

  std::vector<Op> ops_;
  int ip_register_;
  std::vector<int> registers_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day19_2018::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->register_value(0));
}

absl::StatusOr<std::vector<std::string>> Day19_2018::Part2(
    absl::Span<absl::string_view> input) const {
  // Identified another prime factorization assembly code.
  int a = 10551398;
  int z = 0;
  for (int d = 1; d <= a; ++d) {
    if (a % d == 0) z += d;
  }
  return IntReturn(z);
}

}  // namespace advent_of_code
