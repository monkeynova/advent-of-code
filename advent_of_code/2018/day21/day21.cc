// http://adventofcode.com/2018/day/21

#include "advent_of_code/2018/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2018/vm.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool RunDecompiled(absl::Span<absl::string_view> input,
                   absl::FunctionRef<bool(int)> on_test) {
  // Keep the regex strings as a vertical list.
  // clang-format off
  std::vector<std::string> decompiled_re = {
    "#ip 3",
    "seti 123 0 4",
    "bani 4 456 4",
    "eqri 4 72 4",
    "addr 4 3 3",
    "seti 0 0 3",
    "seti 0 6 4",
    "bori 4 65536 1",
    "seti (\\d+) 1 4",
    "bani 1 255 5",
    "addr 4 5 4",
    "bani 4 16777215 4",
    "muli 4 (\\d+) 4",
    "bani 4 16777215 4",
    "gtir 256 1 5",
    "addr 5 3 3",
    "addi 3 1 3",
    "seti 27 8 3",
    "seti 0 1 5",
    "addi 5 1 2",
    "muli 2 256 2",
    "gtrr 2 1 2",
    "addr 2 3 3",
    "addi 3 1 3",
    "seti 25 7 3",
    "addi 5 1 5",
    "seti 17 1 3",
    "setr 5 3 1",
    "seti 7 8 3",
    "eqrr 4 0 5",
    "addr 5 3 3",
    "seti 5 4 3",
  };
  // clang-format on

  std::string full_re = absl::StrJoin(decompiled_re, "\n");
  std::string full_input = absl::StrJoin(input, "\n");
  int p1, p2;
  if (!RE2::FullMatch(full_input, full_re, &p1, &p2)) {
    return false;
  }

  int a, d;
  d = 0;
  do {
    a = d | 0x10000;
    d = p1;
    while (true) {
      d += (a & 0xFF);
      d &= 0xFFFFFF;
      d *= p2;
      d &= 0xFFFFFF;
      if (256 > a) break;
      a /= 256;
    }
    if (on_test(d)) return true;
  } while (d != 0);

  LOG(FATAL) << "Left infinite loop";
}

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

  int last_new = -1;
  absl::flat_hash_set<int> hist;

  bool was_run = RunDecompiled(input, [&](int v) {
    if (hist.contains(v)) return true;
    VLOG(2) << "Value: " << v << " of " << hist.size();
    hist.insert(v);
    last_new = v;
    return false;
  });
  if (was_run) return IntReturn(last_new);

  LOG(ERROR) << "Falling back to interpreted mode...";

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

  absl::Status st = vm->Execute([&](int ip) {
    if (ip == watch_ip) {
      int watch_value = vm->register_value(watch_register);
      if (hist.contains(watch_value)) return true;
      VLOG(2) << "Value: " << watch_value << " of " << hist.size();
      hist.insert(watch_value);
      last_new = watch_value;
    }
    return false;
  });
  if (!st.ok()) return st;

  return IntReturn(last_new);
}

}  // namespace advent_of_code
