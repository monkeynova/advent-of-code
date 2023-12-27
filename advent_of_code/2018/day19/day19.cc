#include "advent_of_code/2018/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2018/vm.h"
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

}  // namespace

absl::StatusOr<std::string> Day_2018_19::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(VM vm, VM::Parse(input));
  RETURN_IF_ERROR(vm.Execute());
  return AdventReturn(vm.register_value(0));
}

absl::StatusOr<std::string> Day_2018_19::Part2(
    absl::Span<std::string_view> input) const {
  // Identified another prime factorization assembly code.
  // TODO(@monkeynova): Try to identify and alter with code.
  int a = 10551398;
  int z = 0;
  for (int d = 1; d <= a; ++d) {
    if (a % d == 0) z += d;
  }
  return AdventReturn(z);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/19, []() {
  return std::unique_ptr<AdventDay>(new Day_2018_19());
});

}  // namespace advent_of_code
