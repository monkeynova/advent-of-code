// https://adventofcode.com/2016/day/23
//
// --- Day 23: Safe Cracking ---
// -----------------------------
// 
// This is one of the top floors of the nicest tower in EBHQ. The Easter
// Bunny's private office is here, complete with a safe hidden behind a
// painting, and who wouldn't hide a star in a safe behind a painting?
// 
// The safe has a digital screen and keypad for code entry. A sticky note
// attached to the safe has a password hint on it: "eggs". The painting
// is of a large rabbit coloring some eggs. You see 7.
// 
// When you go to type the code, though, nothing appears on the display;
// instead, the keypad comes apart in your hands, apparently having been
// smashed. Behind it is some kind of socket - one that matches a
// connector in your prototype computer! You pull apart the smashed
// keypad and extract the logic circuit, plug it into your computer, and
// plug your computer into the safe.
// 
// Now, you just need to figure out what output the keypad would have
// sent to the safe. You extract the assembunny code from the logic chip
// (your puzzle input).
// 
// The code looks like it uses almost the same architecture and
// instruction set that the monorail computer used! You should be able to
// use the same assembunny interpreter for this as you did there, but
// with one new instruction:
// 
// tgl x toggles the instruction x away (pointing at instructions like
// jnz does: positive means forward; negative means backward):
// 
// * For one-argument instructions, inc becomes dec, and all other
// one-argument instructions become inc.
// 
// * For two-argument instructions, jnz becomes cpy, and all other
// two-instructions become jnz.
// 
// * The arguments of a toggled instruction are not affected.
// 
// * If an attempt is made to toggle an instruction outside the
// program, nothing happens.
// 
// * If toggling produces an invalid instruction (like cpy 1 2) and an
// attempt is later made to execute that instruction, skip it instead.
// 
// * If tgl toggles itself (for example, if a is 0, tgl a would target
// itself and become inc a), the resulting instruction is not
// executed until the next time it is reached.
// 
// For example, given this program:
// 
// cpy 2 a
// tgl a
// tgl a
// tgl a
// cpy 1 a
// dec a
// dec a     
// 
// * cpy 2 a initializes register a to 2.
// 
// * The first tgl a toggles an instruction a (2) away from it, which
// changes the third tgl a into inc a.
// 
// * The second tgl a also modifies an instruction 2 away from it,
// which changes the cpy 1 a into jnz 1 a.
// 
// * The fourth line, which is now inc a, increments a to 3.
// 
// * Finally, the fifth line, which is now jnz 1 a, jumps a (3)
// instructions ahead, skipping the dec a instructions.
// 
// In this example, the final value in register a is 3.
// 
// The rest of the electronics seem to place the keypad entry (the number
// of eggs, 7) in register a, run the code, and then send the value left
// in register a to the safe.
// 
// What value should be sent to the safe?
//
// --- Part Two ---
// ----------------
// 
// The safe doesn't open, but it does make several angry noises to
// express its frustration.
// 
// You're quite sure your logic is working correctly, so the only other
// thing is... you check the painting again. As it turns out, colored
// eggs are still eggs. Now you count 12.
// 
// As you run the program with this new input, the prototype computer
// begins to overheat. You wonder what's taking so long, and whether the
// lack of any instruction more powerful than "add one" has anything to
// do with it. Don't bunnies usually multiply?
// 
// Anyway, what value should actually be sent to the safe?


#include "advent_of_code/2016/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

std::optional<int64_t> MatchesDecompiled(absl::Span<std::string_view> input) {
  // Keep the regex strings as a vertical list.
  // clang-format off
  std::vector<absl::string_view> decompilable_res = {
    "cpy a b",
    "dec b",
    "cpy a d",
    "cpy 0 a",
    "cpy b c",
    "inc a",
    "dec c",
    "jnz c \\-2",
    "dec d",
    "jnz d \\-5",
    "dec b",
    "cpy b c",
    "cpy c d",
    "dec d",
    "inc c",
    "jnz d \\-2",
    "tgl c",
    "cpy \\-16 c",
    "jnz 1 c",
    "cpy (\\d+) c",
    "jnz (\\d+) d",
    "inc a",
    "inc d",
    "jnz d \\-2",
    "inc c",
    "jnz c \\-5",
  };
  // clang-format off

  std::string full_re = absl::StrJoin(decompilable_res, "\n");

  int64_t p1, p2;
  if (!RE2::FullMatch(absl::StrJoin(input, "\n"), full_re, &p1, &p2)) {
    LOG(WARNING) << "Could not match re=\n" << full_re;
    LOG(WARNING) << "Against input=\n" << absl::StrJoin(input, "\n");
    return std::nullopt;
  }

  int64_t a = 12;
  for (int64_t b = a - 1; b > 0; --b) {
    a *= b;
  }
  a += p1 * p2;
  return a;
}

absl::StatusOr<std::string> Day_2016_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->registers().a = 7;
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->registers().a);
}

absl::StatusOr<std::string> Day_2016_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (std::optional<int64_t> ret = MatchesDecompiled(input)) {
    return IntReturn(ret);
  }

  LOG(ERROR) << "Falling back to full execution...";

  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->registers().a = 12;
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->registers().a);
}

}  // namespace advent_of_code
