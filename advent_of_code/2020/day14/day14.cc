// https://adventofcode.com/2020/day/14
//
// --- Day 14: Docking Data ---
// ----------------------------
//
// As your ferry approaches the sea port, the captain asks for your help
// again. The computer system that runs this port isn't compatible with
// the docking program on the ferry, so the docking parameters aren't
// being correctly initialized in the docking program's memory.
//
// After a brief inspection, you discover that the sea port's computer
// system uses a strange bitmask system in its initialization program.
// Although you don't have the correct decoder chip handy, you can
// emulate it in software!
//
// The initialization program (your puzzle input) can either update the
// bitmask or write a value to memory. Values and memory addresses are
// both 36-bit unsigned integers. For example, ignoring bitmasks for a
// moment, a line like mem[8] = 11 would write the value 11 to memory
// address 8.
//
// The bitmask is always given as a string of 36 bits, written with the
// most significant bit (representing 2^35) on the left and the least
// significant bit (2^0, that is, the 1s bit) on the right. The current
// bitmask is applied to values immediately before they are written to
// memory: a 0 or 1 overwrites the corresponding bit in the value, while
// an X leaves the bit in the value unchanged.
//
// For example, consider the following program:
//
// mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
// mem[8] = 11
// mem[7] = 101
// mem[8] = 0
//
// This program starts by specifying a bitmask (mask = ....). The mask it
// specifies will overwrite two bits in every written value: the 2s bit
// is overwritten with 0, and the 64s bit is overwritten with 1.
//
// The program then attempts to write the value 11 to memory address 8.
// By expanding everything out to individual bits, the mask is applied as
// follows:
//
// value:  000000000000000000000000000000001011  (decimal 11)
// mask:   XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
// result: 00000000000000000000000000000   1   0010   0   1  (decimal 73)
//
// So, because of the mask, the value 73 is written to memory address 8
// instead. Then, the program tries to write 101 to address 7:
//
// value:  000000000000000000000000000001100101  (decimal 101)
// mask:   XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
// result: 00000000000000000000000000000   1   1001   0   1  (decimal 101)
//
// This time, the mask has no effect, as the bits it overwrote were
// already the values the mask tried to set. Finally, the program tries
// to write 0 to address 8:
//
// value:  000000000000000000000000000000000000  (decimal 0)
// mask:   XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
// result: 00000000000000000000000000000   1   0000   0   0  (decimal 64)
//
// 64 is written to address 8 instead, overwriting the value that was
// there previously.
//
// To initialize your ferry's docking program, you need the sum of all
// values left in memory after the initialization program completes. (The
// entire 36-bit address space begins initialized to the value 0 at every
// address.) In the above example, only two values in memory are not zero
// - 101 (at address 7) and 64 (at address 8) - producing a sum of 165.
//
// Execute the initialization program. What is the sum of all values left
// in memory after it completes? (Do not truncate the sum to 36 bits.)
//
// --- Part Two ---
// ----------------
//
// For some reason, the sea port's computer system still can't
// communicate with your ferry's docking program. It must be using
// version 2 of the decoder chip!
//
// A version 2 decoder chip doesn't modify the values being written at
// all. Instead, it acts as a memory address decoder. Immediately before
// a value is written to memory, each bit in the bitmask modifies the
// corresponding bit of the destination memory address in the following
// way:
//
// * If the bitmask bit is 0, the corresponding memory address bit is
// unchanged.
//
// * If the bitmask bit is 1, the corresponding memory address bit is
// overwritten with 1.
//
// * If the bitmask bit is X, the corresponding memory address bit is
// floating.
//
// A floating bit is not connected to anything and instead fluctuates
// unpredictably. In practice, this means the floating bits will take on
// all possible values, potentially causing many memory addresses to be
// written all at once!
//
// For example, consider the following program:
//
// mask = 000000000000000000000000000000X1001X
// mem[42] = 100
// mask = 00000000000000000000000000000000X0XX
// mem[26] = 1
//
// When this program goes to write to memory address 42, it first applies
// the bitmask:
//
// address: 000000000000000000000000000000101010  (decimal 42)
// mask:    000000000000000000000000000000X1001X
// result:  000000000000000000000000000000   X1   10   1X
//
// After applying the mask, four bits are overwritten, three of which are
// different, and two of which are floating. Floating bits take on every
// possible combination of values; with two floating bits, four actual
// memory addresses are written:
//
// 000000000000000000000000000000   0   1101   0     (decimal 26)
// 000000000000000000000000000000   0   1101   1     (decimal 27)
// 000000000000000000000000000000   1   1101   0     (decimal 58)
// 000000000000000000000000000000   1   1101   1     (decimal 59)
//
// Next, the program is about to write to memory address 26 with a
// different bitmask:
//
// address: 000000000000000000000000000000011010  (decimal 26)
// mask:    00000000000000000000000000000000X0XX
// result:  00000000000000000000000000000001   X   0   XX
//
// This results in an address with three floating bits, causing writes to
// eight memory addresses:
//
// 00000000000000000000000000000001   0   0   00     (decimal 16)
// 00000000000000000000000000000001   0   0   01     (decimal 17)
// 00000000000000000000000000000001   0   0   10     (decimal 18)
// 00000000000000000000000000000001   0   0   11     (decimal 19)
// 00000000000000000000000000000001   1   0   00     (decimal 24)
// 00000000000000000000000000000001   1   0   01     (decimal 25)
// 00000000000000000000000000000001   1   0   10     (decimal 26)
// 00000000000000000000000000000001   1   0   11     (decimal 27)
//
// The entire 36-bit address space still begins initialized to the value
// 0 at every address, and you still need the sum of all values left in
// memory at the end of the program. In this example, the sum is 208.
//
// Execute the initialization program using an emulator for a version 2
// decoder chip. What is the sum of all values left in memory after it
// completes?

#include "advent_of_code/2020/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_14::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int64_t, int64_t> mem;
  int64_t mask = 0;
  int64_t mask_val = 0;
  for (absl::string_view str : input) {
    absl::string_view mask_str;
    int64_t addr;
    int64_t val;
    if (RE2::FullMatch(str, "mask = ([10X]+)", &mask_str)) {
      if (mask_str.size() != 36)
        return Error("Bad mask:", mask_str, "; ", mask_str.size());
      mask = 0;
      mask_val = 0;
      int64_t one_bit = 1;
      for (int i = 0; i < mask_str.size(); ++i) {
        switch (mask_str[i]) {
          case '0': {
            mask |= (one_bit << (35 - i));
            break;
          }
          case '1': {
            mask |= (one_bit << (35 - i));
            mask_val |= (one_bit << (35 - i));
            break;
          }
          case 'X':
            break;
          default:
            return Error("Bad mask char: ", mask_str.substr(i, 1), "; ",
                         mask_str);
        }
      }

    } else if (RE2::FullMatch(str, "mem\\[(\\d+)\\] = (\\d+)", &addr, &val)) {
      mem[addr] = (val & ~mask) | mask_val;
    } else {
      return Error("Bad Input: ", str);
    }
  }
  int64_t sum = 0;
  for (const auto& pair : mem) {
    sum += pair.second;
  }
  return IntReturn(sum);
}

absl::StatusOr<std::string> Day_2020_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int64_t, int64_t> mem;
  int64_t mask = 0;
  int64_t mask_val = 0;
  std::vector<int64_t> floating;
  for (absl::string_view str : input) {
    absl::string_view mask_str;
    int64_t addr;
    int64_t val;
    if (RE2::FullMatch(str, "mask = ([10X]+)", &mask_str)) {
      if (mask_str.size() != 36)
        return Error("Bad mask:", mask_str, "; ", mask_str.size());
      mask = 0;
      mask_val = 0;
      floating.clear();
      int64_t one_bit = 1;
      for (int i = 0; i < mask_str.size(); ++i) {
        switch (mask_str[i]) {
          case '0': {
            // Leave be...
            break;
          }
          case '1': {
            mask |= (one_bit << (35 - i));
            mask_val |= (one_bit << (35 - i));
            break;
          }
          case 'X': {
            mask |= (one_bit << (35 - i));
            if (floating.empty()) {
              floating.push_back(0);
              floating.push_back(one_bit << (35 - i));
            } else {
              int floating_start_size = floating.size();
              for (int j = 0; j < floating_start_size; ++j) {
                floating.push_back(floating[j] | (one_bit << (35 - i)));
              }
            }
            break;
          }
          default:
            return Error("Bad mask char: ", mask_str.substr(i, 1), "; ",
                         mask_str);
        }
      }
      VLOG(1) << absl::StrCat("mask = 0x", absl::Hex(mask),
                              "; mask_val = ", mask_val);
      VLOG(1) << "floating = " << absl::StrJoin(floating, ",");

    } else if (RE2::FullMatch(str, "mem\\[(\\d+)\\] = (\\d+)", &addr, &val)) {
      int64_t base_addr = (addr & ~mask) | mask_val;
      VLOG(1) << "addr = " << (addr);
      VLOG(1) << "addr & ~mask = " << (addr & ~mask);
      VLOG(1) << "((addr & ~mask) | mask_val) = "
              << ((addr & ~mask) | mask_val);
      VLOG(1) << "base_addr = " << base_addr;
      for (int64_t f : floating) {
        mem[base_addr | f] = val;
        VLOG(2) << "Set [" << (base_addr | f) << "]: " << val;
      }
    } else {
      return Error("Bad Input: ", str);
    }
  }
  int64_t sum = 0;
  for (const auto& pair : mem) {
    sum += pair.second;
  }
  return IntReturn(sum);
}

}  // namespace advent_of_code
