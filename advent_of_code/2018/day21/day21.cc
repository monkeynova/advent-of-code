#include "advent_of_code/2018/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2018/vm.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

/*

#ip 3
d = 0
a = d | 65536
d = 678134
do {
  e = a & 255
  d += e
  d &= 16777215
  d *= 65899
  d &= 16777215
  while (a > 256) a /= 256;
  if (a > 256) {
    for (e = 0; (e + 1) * 256 < a; ++e)
      ;// nop
    a = e
    e = 0
    while (true) {
      b = e + 1
      b *= 256
      if (b > a) break;
      e++
    }
    ...
  }
} while (z != d)
17: seti 0 1 5
18: addi 5 1 2
19: muli 2 256 2
20: gtrr 2 1 2
21: addr 2 3 3
22: addi 3 1 3
23: seti 25 7 3   // JMP 25
24: addi 5 1 5
25: seti 17 1 3   // JMP 17
26: setr 5 3 1
27: seti 7 8 3    // JMP 7
28: eqrr 4 0 5
29: addr 5 3 3
30: seti 5 4 3   // JMP 5

#ip 3
00: seti 123 0 4
01: bani 4 456 4
02: eqri 4 72 4
03: addr 4 3 3
04: seti 0 0 3
05: seti 0 6 4
06: bori 4 65536 1
07: seti 678134 1 4
08: bani 1 255 5
09: addr 4 5 4
10: bani 4 16777215 4
11: muli 4 65899 4
12: bani 4 16777215 4
13: gtir 256 1 5
14: addr 5 3 3
15: addi 3 1 3
16: seti 27 8 3
17: seti 0 1 5
18: addi 5 1 2
19: muli 2 256 2
20: gtrr 2 1 2
21: addr 2 3 3
22: addi 3 1 3
23: seti 25 7 3
24: addi 5 1 5
25: seti 17 1 3   // JMP 17
26: setr 5 3 1
27: seti 7 8 3    // JMP 7
28: eqrr 4 0 5
29: addr 5 3 3
30: seti 5 4 3   // JMP 5
*/

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_21::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();

  int watch_register = -1;
  int watch_ip = -1;
  for (int i = 0; i < vm->ops().size(); ++i) {
    const VM::Op& op = vm->ops()[i];
    if (op.op_code == VM::OpCode::kEqrr && (op.arg1 == 0 || op.arg2 == 0)) {
      watch_ip = i;
      watch_register = op.arg1 == 0 ? op.arg2 : op.arg1;
    }
  }
  if (watch_ip == -1) return Error("Could not find equality test");

  int first_test = -1;
  absl::Status st = vm->Execute([&](int ip) {
    if (ip == watch_ip) {
      first_test = vm->register_value(watch_register);
      return true;
    }
    return false;
  });
  if (!st.ok()) return st;

  return IntReturn(first_test);
}

absl::StatusOr<std::string> Day_2018_21::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();

  int watch_register = -1;
  int watch_ip = -1;
  for (int i = 0; i < vm->ops().size(); ++i) {
    const VM::Op& op = vm->ops()[i];
    if (op.op_code == VM::OpCode::kEqrr && (op.arg1 == 0 || op.arg2 == 0)) {
      watch_ip = i;
      watch_register = op.arg1 == 0 ? op.arg2 : op.arg1;
    }
  }
  if (watch_ip == -1) return Error("Could not find equality test");

  int last_new = -1;
  absl::flat_hash_set<int> hist;
  absl::Status st = vm->Execute([&](int ip) {
    if (ip == watch_ip) {
      int watch_value = vm->register_value(watch_register);
      if (hist.contains(watch_value)) return true;
      VLOG_IF(1, hist.size() % 777 == 0)
          << "Value: " << watch_value << " of " << hist.size();
      hist.insert(watch_value);
      last_new = watch_value;
    }
    return false;
  });
  if (!st.ok()) return st;

  return IntReturn(last_new);
}

}  // namespace advent_of_code
