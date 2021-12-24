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

struct VM {
  static VM Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view line : input) {
      ret.ops_.push_back(std::vector<absl::string_view>(absl::StrSplit(line, " ")));
    }
    return ret;
  }

  absl::Status Execute(std::string input) {
    Reset();
    auto input_it = input.begin();
    for (const auto& op : ops_) {
      VLOG(2) << "Registers: w: " << registers["w"] << " x:" << registers["x"]
              << " y: " << registers["y"] << " z: " << registers["z"];
      VLOG(2) << "Op: " << absl::StrJoin(op, " ");
      auto it_dest = registers.find(op[1]);
      if (it_dest == registers.end()) return Error("Bad dest");
      if (op[0] == "inp") {
        if (op.size() != 2) return Error("Bad inp");
        if (input_it == input.end()) return Error("Past input");
        it_dest->second = *input_it - '0';
        ++input_it;
      } else if (op[0] == "add") {
        if (op.size() != 3) return Error("Bad inp");
        auto it_src = registers.find(op[2]);
        if (it_src != registers.end()) {
          it_dest->second += it_src->second;
        } else {
          int literal;
          if (!absl::SimpleAtoi(op[2], &literal)) return Error("Bad literal");
          it_dest->second += literal;
        }
      } else if (op[0] == "mul") {
        if (op.size() != 3) return Error("Bad inp");
        auto it_src = registers.find(op[2]);
        if (it_src != registers.end()) {
          it_dest->second *= it_src->second;
        } else {
          int literal;
          if (!absl::SimpleAtoi(op[2], &literal)) return Error("Bad literal");
          it_dest->second *= literal;
        }
      } else if (op[0] == "div") {
        if (op.size() != 3) return Error("Bad inp");
        auto it_src = registers.find(op[2]);
        if (it_src != registers.end()) {
          it_dest->second /= it_src->second;
        } else {
          int literal;
          if (!absl::SimpleAtoi(op[2], &literal)) return Error("Bad literal");
          it_dest->second /= literal;
        }
      } else if (op[0] == "mod") {
        if (op.size() != 3) return Error("Bad inp");
        auto it_src = registers.find(op[2]);
        if (it_src != registers.end()) {
          it_dest->second %= it_src->second;
        } else {
          int literal;
          if (!absl::SimpleAtoi(op[2], &literal)) return Error("Bad literal");
          it_dest->second %= literal;
        }
      } else if (op[0] == "eql") {
        if (op.size() != 3) return Error("Bad inp");
        auto it_src = registers.find(op[2]);
        if (it_src != registers.end()) {
          it_dest->second = it_dest->second == it_src->second;
        } else {
          int literal;
          if (!absl::SimpleAtoi(op[2], &literal)) return Error("Bad literal");
          it_dest->second = it_dest->second == literal;
        }
      }
    }
    if (input_it != input.end()) return Error("Bad input");
    return absl::OkStatus();
  }

  void Reset() {
    for (auto& [name, val] : registers) {
      val = 0;
    }
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
    int64_t save_z = z;
    z /= z_array[i];
    if ((save_z % 26) + x_array[i] != input[i] - '0') {
      z *= 26;
      z += (input[i] - '0') + y_array[i];
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
  int64_t save_z = z;
  z /= z_array[off];
  if ((save_z % 26) + x_array[off] != (input - '0')) {
    z *= 26;
    z += (input - '0') + y_array[off];
  }
  return z;
}

bool BetterInputMax(const std::string& a, const std::string& b) {
  CHECK_EQ(a.size(), b.size());
  return a > b;
}

bool BetterInputMin(const std::string& a, const std::string& b) {
  CHECK_EQ(a.size(), b.size());
  return a < b;
}

std::string FindBestInput(absl::FunctionRef<bool(const std::string&, const std::string&)> better_input_p) {
  absl::flat_hash_map<int64_t, std::string> z_to_partial = {
    {0, {}},
  };
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
    if (vm.registers["z"] != Decompiled(test)) {
      return Error("Bad Decompiled: sample");
    }
  }
  if (run_audit()) {
    std::string test = "29989297949519";
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.registers["z"] != 0) return Error("Not 0 checksum");
    if (Decompiled(test) != 0) return Error("Not 0 checksum (Decompiled)");
  }
  return FindBestInput(BetterInputMax);
}

absl::StatusOr<std::string> Day_2021_24::Part2(
    absl::Span<absl::string_view> input) const {
  VM vm = VM::Parse(input);
  if (run_audit()) {
    std::string test = "19518121316118";
    vm.Reset();
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.registers["z"] != 0) return Error("Not 0 checksum");
    if (Decompiled(test) != 0) return Error("Not 0 checksum (Decompiled)");
  }
  return FindBestInput(BetterInputMin);
}

}  // namespace advent_of_code
