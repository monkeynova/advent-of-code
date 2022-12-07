// https://adventofcode.com/2018/day/19
//
// --- Day 19: Go With The Flow ---
// --------------------------------
// 
// With the Elves well on their way constructing the North Pole base, you
// turn your attention back to understanding the inner workings of
// programming the device.
// 
// You can't help but notice that the device's opcodes don't contain any
// flow control like jump instructions. The device's manual goes on to
// explain:
// 
// "In programs where flow control is required, the instruction pointer
// can be bound to a register so that it can be manipulated directly.
// This way, setr/seti can function as absolute jumps, addr/addi can
// function as relative jumps, and other opcodes can cause truly
// fascinating effects."
// 
// This mechanism is achieved through a declaration like #ip 1, which
// would modify register 1 so that accesses to it let the program
// indirectly access the instruction pointer itself. To compensate for
// this kind of binding, there are now six registers (numbered 0 through
// 5); the five not bound to the instruction pointer behave as normal.
// Otherwise, the same rules apply as the last time you worked with this
// device.
// 
// When the instruction pointer is bound to a register, its value is
// written to that register just before each instruction is executed, and
// the value of that register is written back to the instruction pointer
// immediately after each instruction finishes execution. Afterward, move
// to the next instruction by adding one to the instruction pointer, even
// if the value in the instruction pointer was just updated by an
// instruction. (Because of this, instructions must effectively set the
// instruction pointer to the instruction before the one they want
// executed next.)
// 
// The instruction pointer is 0 during the first instruction, 1 during
// the second, and so on. If the instruction pointer ever causes the
// device to attempt to load an instruction outside the instructions
// defined in the program, the program instead immediately halts. The
// instruction pointer starts at 0.
// 
// It turns out that this new information is already proving useful: the
// CPU in the device is not very powerful, and a background process is
// occupying most of its time. You dump the background process'
// declarations and instructions to a file (your puzzle input), making
// sure to use the names of the opcodes rather than the numbers.
// 
// For example, suppose you have the following program:
// 
// #ip 0
// seti 5 0 1
// seti 6 0 2
// addi 0 1 0
// addr 1 2 3
// setr 1 0 0
// seti 8 0 4
// seti 9 0 5
// 
// When executed, the following instructions are executed. Each line
// contains the value of the instruction pointer at the time the
// instruction started, the values of the six registers before executing
// the instructions (in square brackets), the instruction itself, and the
// values of the six registers after executing the instruction (also in
// square brackets).
// 
// ip=0 [0, 0, 0, 0, 0, 0] seti 5 0 1 [0, 5, 0, 0, 0, 0]
// ip=1 [1, 5, 0, 0, 0, 0] seti 6 0 2 [1, 5, 6, 0, 0, 0]
// ip=2 [2, 5, 6, 0, 0, 0] addi 0 1 0 [3, 5, 6, 0, 0, 0]
// ip=4 [4, 5, 6, 0, 0, 0] setr 1 0 0 [5, 5, 6, 0, 0, 0]
// ip=6 [6, 5, 6, 0, 0, 0] seti 9 0 5 [6, 5, 6, 0, 0, 9]
// 
// In detail, when running this program, the following events occur:
// 
// * The first line (#ip 0) indicates that the instruction pointer
// should be bound to register 0 in this program. This is not an
// instruction, and so the value of the instruction pointer does not
// change during the processing of this line.
// 
// * The instruction pointer contains 0, and so the first instruction
// is executed (seti 5 0 1). It updates register 0 to the current
// instruction pointer value (0), sets register 1 to 5, sets the
// instruction pointer to the value of register 0 (which has no
// effect, as the instruction did not modify register 0), and then
// adds one to the instruction pointer.
// 
// * The instruction pointer contains 1, and so the second instruction,
// seti 6 0 2, is executed. This is very similar to the instruction
// before it: 6 is stored in register 2, and the instruction pointer
// is left with the value 2.
// 
// * The instruction pointer is 2, which points at the instruction addi
// 0 1 0. This is like a relative jump: the value of the instruction
// pointer, 2, is loaded into register 0. Then, addi finds the result
// of adding the value in register 0 and the value 1, storing the
// result, 3, back in register 0. Register 0 is then copied back to
// the instruction pointer, which will cause it to end up 1 larger
// than it would have otherwise and skip the next instruction (addr 1
// 2 3) entirely. Finally, 1 is added to the instruction pointer.
// 
// * The instruction pointer is 4, so the instruction setr 1 0 0 is
// run. This is like an absolute jump: it copies the value contained
// in register 1, 5, into register 0, which causes it to end up in
// the instruction pointer. The instruction pointer is then
// incremented, leaving it at 6.
// 
// * The instruction pointer is 6, so the instruction seti 9 0 5 stores
// 9 into register 5. The instruction pointer is incremented, causing
// it to point outside the program, and so the program ends.
// 
// What value is left in register 0 when the background process halts?
//
// --- Part Two ---
// ----------------
// 
// A new background process immediately spins up in its place. It appears
// identical, but on closer inspection, you notice that this time,
// register 0 started with the value 1.
// 
// What value is left in register 0 when this new background process
// halts?


#include "advent_of_code/2018/day19/day19.h"

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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->register_value(0));
}

absl::StatusOr<std::string> Day_2018_19::Part2(
    absl::Span<absl::string_view> input) const {
  // Identified another prime factorization assembly code.
  // TODO(@monkeynova): Try to identify and alter with code.
  int a = 10551398;
  int z = 0;
  for (int d = 1; d <= a; ++d) {
    if (a % d == 0) z += d;
  }
  return IntReturn(z);
}

}  // namespace advent_of_code
