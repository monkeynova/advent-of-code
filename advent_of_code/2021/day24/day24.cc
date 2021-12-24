#include "advent_of_code/2021/day24/day24.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/functional/function_ref.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class VM {
 public:
  static VM Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view line : input) {
      ret.ops_.push_back(std::vector<absl::string_view>(absl::StrSplit(line, " ")));
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
    {"w", 0}, {"x", 0}, {"y", 0}, {"z", 0},
  };
};

int64_t Decompiled(std::string input) {
  CHECK_EQ(input.size(), 14);
  int64_t z = 0;
  std::vector<int64_t> x_array = {
    14, 14, 14, 12, 15, -12, -12, 12, -7, 18, -8, -5, -10, -7
  };
  std::vector<int64_t> y_array = {
    14, 2, 1, 13, 5, 5, 5, 9, 3, 13, 2, 1, 11, 8 
  };
  std::vector<int64_t> z_array = {
    1, 1, 1, 1, 1, 26, 26, 1, 26, 1, 26, 26, 26, 26,
  };
  for (int i = 0; i < 14; ++i) {
    if ((z % 26) + x_array[i] != input[i] - '0') {
      z /= z_array[i];
      z *= 26;
      z += (input[i] - '0') + y_array[i];
    } else {
      z /= z_array[i];
    }
  }
  return z;
}

int64_t DecompiledPartial(char input, int64_t off, int64_t z) {
  std::vector<int64_t> x_array = {
    14, 14, 14, 12, 15, -12, -12, 12, -7, 18, -8, -5, -10, -7
  };
  std::vector<int64_t> y_array = {
    14, 2, 1, 13, 5, 5, 5, 9, 3, 13, 2, 1, 11, 8 
  };
  std::vector<int64_t> z_array = {
    1, 1, 1, 1, 1, 26, 26, 1, 26, 1, 26, 26, 26, 26,
  };
  if ((z % 26) + x_array[off] != (input - '0')) {
    z /= z_array[off];
    z *= 26;
    z += (input - '0') + y_array[off];
  } else {
    z /= z_array[off];
  }
  return z;
}

absl::flat_hash_set<std::pair<int64_t, char>> ReversePartial(int64_t off, const int64_t z) {
  std::vector<int64_t> x_array = {
    14, 14, 14, 12, 15, -12, -12, 12, -7, 18, -8, -5, -10, -7
  };
  std::vector<int64_t> y_array = {
    14, 2, 1, 13, 5, 5, 5, 9, 3, 13, 2, 1, 11, 8 
  };
  std::vector<int64_t> z_array = {
    1, 1, 1, 1, 1, 26, 26, 1, 26, 1, 26, 26, 26, 26,
  };

  absl::flat_hash_set<std::pair<int64_t, char>> ret;
  for (char input = '1'; input <= '9'; ++input) {
    absl::string_view input_view(&input, 1);
    int64_t prev_mod_26 = (input - '0') - x_array[off];
    {
      // if is true
      int64_t this_z = z;
      this_z -= (input - '0') + y_array[off];
      if (this_z % 26 == 0) {
        // Otherwise couldn't have gotten to this `z` with `input`.
        this_z /= 26;
        if (z_array[off] == 1) {
          if (this_z % 26 != prev_mod_26) {
            VLOG(3) << "Adding: " << this_z << ", " << input_view;
            ret.emplace(this_z, input);
          }
        } else {
          CHECK_EQ(z_array[off], 26);
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
      if (z_array[off] == 1) {
        if (this_z % 26 == prev_mod_26) {
          VLOG(3) << "Adding: " << this_z << ", " << input_view;
          ret.emplace(this_z, input);
        }
      } else {
        CHECK_EQ(z_array[off], 26);
        VLOG(3) << "Adding: " << 26 * this_z + prev_mod_26 << ", " << input_view;
        ret.emplace(26 * this_z + prev_mod_26, input);
      }
    }
  }

  if (false) {
    // Check precision...
    for (const auto& [this_z, i] : ret) {
      VLOG(3) << this_z << "/" << absl::string_view(&i, 1);
      int64_t next_z = DecompiledPartial(i, off, this_z);
      VLOG(3) << next_z << "/" << z;
      CHECK_EQ(next_z, z)
          << "[" << this_z << ", " << absl::string_view(&i, 1) << "]";
    }
  }
  if (false) {
    // Check recall...
    for (int this_z = z / 26; this_z < 27 * z; ++this_z) {
      for (char i = '1'; i <= '9'; ++i) {
        int64_t next_z = DecompiledPartial(i, off, this_z);
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

std::string FindBestInputReverse(BetterInputP better_input_p) {
  absl::flat_hash_map<int64_t, std::vector<std::string>> z_to_input = {{0, {""}}};
  for (int offset = 13; offset >= 0; --offset) {
    VLOG(1) << offset << ": " << z_to_input.size();
    int total_inputs = 0;
    absl::flat_hash_map<int64_t, std::vector<std::string>> new_z_to_input;
    for (const auto& [z, input_list] : z_to_input) {
      absl::flat_hash_set<std::pair<int64_t, char>> next = ReversePartial(offset, z);
      for (const auto& [z, c] : next) {
        for (const auto& input : input_list) {
          std::string next_input = std::string(1, c) + input;
          new_z_to_input[z].push_back(next_input);
          ++total_inputs;
        }
      }
    }
    VLOG(1) << "  Generated " << total_inputs << " inputs";
    z_to_input = std::move(new_z_to_input);
  }
  VLOG(1) << "Final: " << z_to_input.size();
  VLOG(1) << "Contains 0 " << z_to_input.contains(0);
  auto it = z_to_input.find(0);
  CHECK(it != z_to_input.end());
  CHECK(!it->second.empty());
  std::string ret = *it->second.begin();
  for (const auto& s : it->second) {
    if (better_input_p(s, ret)) {
      ret = s;
    }
  }
  VLOG(1) << ret;
  return ret;

}

std::string FindBestInput(BetterInputP better_input_p) {
  absl::flat_hash_map<int64_t, std::string> z_to_partial = {{0, {}}};
  for (int offset = 0; offset < 14; ++offset) {
    VLOG(1) << offset << ": " << z_to_partial.size();
    absl::flat_hash_map<int64_t, std::string> next_z_to_partial;
    for (const auto& [z, this_input] : z_to_partial) {
      std::string next_input = this_input;
      for (char input = '1'; input <= '9'; ++input) {
        int64_t next_z = DecompiledPartial(input, offset, z);
        next_input.append(1, input);
        CHECK_EQ(next_input.size(), offset + 1);
        auto [it, inserted] = next_z_to_partial.emplace(next_z, next_input);
        if (!inserted) {
          if (better_input_p(next_input, it->second)) {
            it->second = next_input;
          }
        }
        next_input.resize(this_input.size());
      }
    }
    z_to_partial = std::move(next_z_to_partial);
  }
  VLOG(1) << "Final: " << z_to_partial.size();
  VLOG(1) << "Contains 0 " << z_to_partial.contains(0);
  auto it = z_to_partial.find(0);
  CHECK(it != z_to_partial.end());
  std::string ret = it->second;
  VLOG(1) << ret;
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_24::Part1(
    absl::Span<absl::string_view> input) const {
  VM vm = VM::Parse(input);
  if (run_audit()) {
    std::string test = "13579246899999";
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.RegisterValue("z") != Decompiled(test)) {
      return Error("Bad Decompiled: sample");
    }
  }
  if (run_audit()) {
    std::string test = "29989297949519";
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.RegisterValue("z") != 0) return Error("Not 0 checksum");
    if (Decompiled(test) != 0) return Error("Not 0 checksum (Decompiled)");
  }
  return FindBestInputReverse(BetterInputMax);
  return FindBestInput(BetterInputMax);
}

absl::StatusOr<std::string> Day_2021_24::Part2(
    absl::Span<absl::string_view> input) const {
  VM vm = VM::Parse(input);
  if (run_audit()) {
    std::string test = "19518121316118";
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.RegisterValue("z") != 0) return Error("Not 0 checksum");
    if (Decompiled(test) != 0) return Error("Not 0 checksum (Decompiled)");
  }
  return FindBestInputReverse(BetterInputMin);
  return FindBestInput(BetterInputMin);
}

}  // namespace advent_of_code
