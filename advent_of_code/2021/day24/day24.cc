// https://adventofcode.com/2021/day/24
//
// --- Day 24: Arithmetic Logic Unit ---
// -------------------------------------
//
// Magic smoke starts leaking from the submarine's arithmetic logic unit
// (ALU). Without the ability to perform basic arithmetic and logic
// functions, the submarine can't produce cool patterns with its
// Christmas lights!
//
// It also can't navigate. Or run the oxygen system.
//
// Don't worry, though - you probably have enough oxygen left to give you
// enough time to build a new ALU.
//
// The ALU is a four-dimensional processing unit: it has integer
// variables w, x, y, and z. These variables all start with the value 0.
// The ALU also supports six instructions:
//
// * inp a - Read an input value and write it to variable a.
//
// * add a b - Add the value of a to the value of b, then store the
// result in variable a.
//
// * mul a b - Multiply the value of a by the value of b, then store
// the result in variable a.
//
// * div a b - Divide the value of a by the value of b, truncate the
// result to an integer, then store the result in variable a. (Here,
// "truncate" means to round the value toward zero.)
//
// * mod a b - Divide the value of a by the value of b, then store the
// remainder in variable a. (This is also called the modulo
// operation.)
//
// * eql a b - If the value of a and b are equal, then store the value
// 1 in variable a. Otherwise, store the value 0 in variable a.
//
// In all of these instructions, a and b are placeholders; a will always
// be the variable where the result of the operation is stored (one of w,
// x, y, or z), while b can be either a variable or a number. Numbers can
// be positive or negative, but will always be integers.
//
// The ALU has no jump instructions; in an ALU program, every instruction
// is run exactly once in order from top to bottom. The program halts
// after the last instruction has finished executing.
//
// (Program authors should be especially cautious; attempting to execute
// div with b=0 or attempting to execute mod with a<0 or b<=0 will cause
// the program to crash and might even damage the ALU. These operations
// are never intended in any serious ALU program.)
//
// For example, here is an ALU program which takes an input number,
// negates it, and stores it in x:
//
// inp x
// mul x -1
//
// Here is an ALU program which takes two input numbers, then sets z to 1
// if the second input number is three times larger than the first input
// number, or sets z to 0 otherwise:
//
// inp z
// inp x
// mul z 3
// eql z x
//
// Here is an ALU program which takes a non-negative integer as input,
// converts it into binary, and stores the lowest (1's) bit in z, the
// second-lowest (2's) bit in y, the third-lowest (4's) bit in x, and the
// fourth-lowest (8's) bit in w:
//
// inp w
// add z w
// mod z 2
// div w 2
// add y w
// mod y 2
// div w 2
// add x w
// mod x 2
// div w 2
// mod w 2
//
// Once you have built a replacement ALU, you can install it in the
// submarine, which will immediately resume what it was doing when the
// ALU failed: validating the submarine's model number. To do this, the
// ALU will run the MOdel Number Automatic Detector program (MONAD, your
// puzzle input).
//
// Submarine model numbers are always fourteen-digit numbers consisting
// only of digits 1 through 9. The digit 0 cannot appear in a model
// number.
//
// When MONAD checks a hypothetical fourteen-digit model number, it uses
// fourteen separate inp instructions, each expecting a single digit of
// the model number in order of most to least significant. (So, to check
// the model number 13579246899999, you would give 1 to the first inp
// instruction, 3 to the second inp instruction, 5 to the third inp
// instruction, and so on.) This means that when operating MONAD, each
// input instruction should only ever be given an integer value of at
// least 1 and at most 9.
//
// Then, after MONAD has finished running all of its instructions, it
// will indicate that the model number was valid by leaving a 0 in
// variable z. However, if the model number was invalid, it will leave
// some other non-zero value in z.
//
// MONAD imposes additional, mysterious restrictions on model numbers,
// and legend says the last copy of the MONAD documentation was eaten by
// a tanuki. You'll need to figure out what MONAD does some other way.
//
// To enable as many submarine features as possible, find the largest
// valid fourteen-digit model number that contains no 0 digits. What is
// the largest model number accepted by MONAD?
//
// --- Part Two ---
// ----------------
//
// As the submarine starts booting up things like the Retro Encabulator,
// you realize that maybe you don't need all these submarine features
// after all.
//
// What is the smallest model number accepted by MONAD?

#include "advent_of_code/2021/day24/day24.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/functional/function_ref.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class VM {
 public:
  static VM Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view line : input) {
      ret.ops_.push_back(
          std::vector<absl::string_view>(absl::StrSplit(line, " ")));
    }
    return ret;
  }

  int64_t RegisterValue(absl::string_view name) const {
    auto it = registers.find(name);
    if (it == registers.end()) return 0;
    return it->second;
  }

  absl::Status Execute(std::string input) {
    Reset();
    auto input_it = input.begin();
    for (const auto& op : ops_) {
      VLOG(2) << "Registers: w: " << registers["w"] << " x:" << registers["x"]
              << " y: " << registers["y"] << " z: " << registers["z"];
      VLOG(2) << "Op: " << absl::StrJoin(op, " ");
      int64_t* dest = FindRegister(op[1]);
      if (dest == nullptr) return Error("Bad dest: ", op[1]);
      if (op[0] == "inp") {
        if (op.size() != 2) return Error("Bad inp");
        if (input_it == input.end()) return Error("Past input");
        *dest = *input_it - '0';
        ++input_it;
      } else if (op[0] == "add") {
        if (op.size() != 3) return Error("Bad inp");
        absl::StatusOr<int64_t> src = RegisterOrLiteral(op[2]);
        if (!src.ok()) return src.status();
        *dest += *src;
      } else if (op[0] == "mul") {
        if (op.size() != 3) return Error("Bad inp");
        absl::StatusOr<int64_t> src = RegisterOrLiteral(op[2]);
        if (!src.ok()) return src.status();
        *dest *= *src;
      } else if (op[0] == "div") {
        if (op.size() != 3) return Error("Bad inp");
        absl::StatusOr<int64_t> src = RegisterOrLiteral(op[2]);
        if (!src.ok()) return src.status();
        *dest /= *src;
      } else if (op[0] == "mod") {
        if (op.size() != 3) return Error("Bad inp");
        absl::StatusOr<int64_t> src = RegisterOrLiteral(op[2]);
        if (!src.ok()) return src.status();
        *dest %= *src;
      } else if (op[0] == "eql") {
        absl::StatusOr<int64_t> src = RegisterOrLiteral(op[2]);
        if (!src.ok()) return src.status();
        *dest = (*dest == *src);
      } else {
        return Error("Unsupported op: ", op[0]);
      }
    }
    if (input_it != input.end()) return Error("Bad input");
    return absl::OkStatus();
  }

 private:
  void Reset() {
    for (auto& [name, val] : registers) {
      val = 0;
    }
  }

  int64_t* FindRegister(absl::string_view name) {
    auto it = registers.find(name);
    if (it == registers.end()) return nullptr;
    return &it->second;
  }

  absl::StatusOr<int64_t> RegisterOrLiteral(absl::string_view name) const {
    auto it = registers.find(name);
    if (it != registers.end()) {
      return it->second;
    }
    int64_t literal;
    if (!absl::SimpleAtoi(name, &literal)) {
      return Error("Bad literal: ", name);
    }
    return literal;
  }

  std::vector<std::vector<absl::string_view>> ops_;
  absl::flat_hash_map<absl::string_view, int64_t> registers = {
      {"w", 0},
      {"x", 0},
      {"y", 0},
      {"z", 0},
  };
};

struct DecompiledConstants {
  std::vector<int64_t> x_array;
  std::vector<int64_t> y_array;
  std::vector<int64_t> z_array;
};

absl::StatusOr<DecompiledConstants> ExtractConstants(
    absl::Span<absl::string_view> input) {
  std::vector<std::string> per_input = {
      "inp w",          "mul x 0", "add x z", "mod x 26", "div z (1|26)",
      "add x (-?\\d+)", "eql x w", "eql x 0", "mul y 0",  "add y 25",
      "mul y x",        "add y 1", "mul z y", "mul y 0",  "add y w",
      "add y (-?\\d+)", "mul y x", "add z y",
  };
  if (input.size() != 14 * per_input.size()) return Error("Cannot match input");
  DecompiledConstants ret;
  for (int i = 0; i < 14; ++i) {
    int input_off = i * per_input.size();
    for (int j = 0; j < per_input.size(); ++j) {
      if (!RE2::FullMatch(input[input_off + j], per_input[j])) {
        return Error("Cannot match input: ", input[input_off + j], " !~ ",
                     per_input[j]);
      }
    }
    int z_val;
    if (!RE2::FullMatch(input[input_off + 4], per_input[4], &z_val)) {
      return Error("Integrity check");
    }
    ret.z_array.push_back(z_val);
    int x_val;
    if (!RE2::FullMatch(input[input_off + 5], per_input[5], &x_val)) {
      return Error("Integrity check");
    }
    ret.x_array.push_back(x_val);
    int y_val;
    if (!RE2::FullMatch(input[input_off + 15], per_input[15], &y_val)) {
      return Error("Integrity check");
    }
    ret.y_array.push_back(y_val);
  }
  return ret;
}

int64_t Decompiled(const DecompiledConstants& dc, std::string input) {
  CHECK_EQ(input.size(), 14);
  int64_t z = 0;
  for (int i = 0; i < 14; ++i) {
    if ((z % 26) + dc.x_array[i] != input[i] - '0') {
      z /= dc.z_array[i];
      z *= 26;
      z += (input[i] - '0') + dc.y_array[i];
    } else {
      z /= dc.z_array[i];
    }
  }
  return z;
}

int64_t DecompiledPartial(const DecompiledConstants& dc, char input,
                          int64_t off, int64_t z) {
  if ((z % 26) + dc.x_array[off] != (input - '0')) {
    z /= dc.z_array[off];
    z *= 26;
    z += (input - '0') + dc.y_array[off];
  } else {
    z /= dc.z_array[off];
  }
  return z;
}

absl::flat_hash_set<std::pair<int64_t, char>> ReversePartial(
    const DecompiledConstants& dc, int64_t off, const int64_t z) {
  absl::flat_hash_set<std::pair<int64_t, char>> ret;
  for (char input = '1'; input <= '9'; ++input) {
    absl::string_view input_view(&input, 1);
    int64_t prev_mod_26 = (input - '0') - dc.x_array[off];
    {
      // if is true
      int64_t this_z = z;
      this_z -= (input - '0') + dc.y_array[off];
      if (this_z % 26 == 0) {
        // Otherwise couldn't have gotten to this `z` with `input`.
        this_z /= 26;
        if (dc.z_array[off] == 1) {
          if (this_z % 26 != prev_mod_26) {
            VLOG(3) << "Adding: " << this_z << ", " << input_view;
            ret.emplace(this_z, input);
          }
        } else {
          CHECK_EQ(dc.z_array[off], 26);
          for (int mod_26 = 0; mod_26 < 26; ++mod_26) {
            if (mod_26 == prev_mod_26) continue;
            VLOG(3) << "Adding: " << this_z * 26 + mod_26 << ", " << input_view;
            ret.emplace(this_z * 26 + mod_26, input);
          }
        }
      }
    }
    {
      // if is false
      int64_t this_z = z;
      if (dc.z_array[off] == 1) {
        if (this_z % 26 == prev_mod_26) {
          VLOG(3) << "Adding: " << this_z << ", " << input_view;
          ret.emplace(this_z, input);
        }
      } else {
        CHECK_EQ(dc.z_array[off], 26);
        VLOG(3) << "Adding: " << 26 * this_z + prev_mod_26 << ", "
                << input_view;
        ret.emplace(26 * this_z + prev_mod_26, input);
      }
    }
  }

  if (false) {
    // Check precision...
    for (const auto& [this_z, i] : ret) {
      VLOG(3) << this_z << "/" << absl::string_view(&i, 1);
      int64_t next_z = DecompiledPartial(dc, i, off, this_z);
      VLOG(3) << next_z << "/" << z;
      CHECK_EQ(next_z, z) << "[" << this_z << ", " << absl::string_view(&i, 1)
                          << "]";
    }
  }
  if (false) {
    // Check recall...
    for (int this_z = z / 26; this_z < 27 * z; ++this_z) {
      for (char i = '1'; i <= '9'; ++i) {
        int64_t next_z = DecompiledPartial(dc, i, off, this_z);
        if (next_z == z) {
          CHECK(ret.contains(std::make_pair(this_z, i)))
              << "[" << this_z << ", " << absl::string_view(&i, 1) << "]";
        }
      }
    }
  }

  return ret;
}

bool BetterInputMax(const std::string& a, const std::string& b) {
  CHECK_EQ(a.size(), b.size());
  return a > b;
}

bool BetterInputMin(const std::string& a, const std::string& b) {
  CHECK_EQ(a.size(), b.size());
  return a < b;
}

using BetterInputP =
    absl::FunctionRef<bool(const std::string&, const std::string&)>;

std::string FindBestInputMiddle(const DecompiledConstants& dc,
                                BetterInputP better_input_p) {
  // Calculate back from the end to the middle.
  absl::flat_hash_map<int64_t, std::vector<std::string>> end_map = {{0, {""}}};
  for (int offset = 13; offset >= 7; --offset) {
    VLOG(1) << offset << ": " << end_map.size();
    int total_inputs = 0;
    absl::flat_hash_map<int64_t, std::vector<std::string>> new_end_map;
    for (const auto& [z, input_list] : end_map) {
      absl::flat_hash_set<std::pair<int64_t, char>> next =
          ReversePartial(dc, offset, z);
      for (const auto& [z, c] : next) {
        for (const auto& input : input_list) {
          std::string next_input = std::string(1, c) + input;
          new_end_map[z].push_back(next_input);
          ++total_inputs;
        }
      }
    }
    VLOG(1) << "  Generated " << total_inputs << " inputs";
    end_map = std::move(new_end_map);
  }

  absl::flat_hash_map<int64_t, std::string> head_map = {{0, {}}};
  for (int offset = 0; offset < 7; ++offset) {
    VLOG(1) << offset << ": " << head_map.size();
    absl::flat_hash_map<int64_t, std::string> next_head_map;
    for (const auto& [z, this_input] : head_map) {
      std::string next_input = this_input;
      for (char input = '1'; input <= '9'; ++input) {
        int64_t next_z = DecompiledPartial(dc, input, offset, z);
        next_input.append(1, input);
        CHECK_EQ(next_input.size(), offset + 1);
        auto [it, inserted] = next_head_map.emplace(next_z, next_input);
        if (!inserted) {
          if (better_input_p(next_input, it->second)) {
            it->second = next_input;
          }
        }
        next_input.resize(this_input.size());
      }
    }
    head_map = std::move(next_head_map);
  }

  std::string ret;
  for (const auto& [z, tail] : end_map) {
    auto it = head_map.find(z);
    if (it == head_map.end()) continue;
    std::string head_input = it->second;
    for (std::string tail_input : tail) {
      std::string test = head_input + tail_input;
      if (ret.empty() || better_input_p(test, ret)) {
        ret = test;
      }
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DecompiledConstants> dc = ExtractConstants(input);
  if (!dc.ok()) return dc.status();
  std::string ret = FindBestInputMiddle(*dc, BetterInputMax);
  VM vm = VM::Parse(input);
  if (auto st = vm.Execute(ret); !st.ok()) return st;
  if (vm.RegisterValue("z") != 0) return Error("VM Check failure");
  return ret;
}

absl::StatusOr<std::string> Day_2021_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DecompiledConstants> dc = ExtractConstants(input);
  if (!dc.ok()) return dc.status();
  std::string ret = FindBestInputMiddle(*dc, BetterInputMin);
  VM vm = VM::Parse(input);
  if (auto st = vm.Execute(ret); !st.ok()) return st;
  if (vm.RegisterValue("z") != 0) return Error("VM Check failure");
  return ret;
}

}  // namespace advent_of_code
