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

  absl::Status Execute(std::vector<int> input) {
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
        it_dest->second = *input_it;
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

int64_t Decompiled(std::vector<int> input) {
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
    if ((save_z % 26) + x_array[i] != input[i]) {
      z *= 26;
      z += input[i] + y_array[i];
    }
  }
  return z;
}

int64_t DecompiledPartial(int input, int64_t off, int64_t z) {
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
  if ((save_z % 26) + x_array[off] != input) {
    z *= 26;
    z += input + y_array[off];
  }
  return z;
}

bool BetterInputMax(const std::vector<int>& a, const std::vector<int>& b) {
  CHECK_EQ(a.size(), b.size());
  for (int i = 0; i < a.size(); ++i) {
    if (a[i] > b[i]) return true;
    if (a[i] < b[i]) return false;
  }
  return false;
}

bool BetterInputMin(const std::vector<int>& a, const std::vector<int>& b) {
  CHECK_EQ(a.size(), b.size());
  for (int i = 0; i < a.size(); ++i) {
    if (a[i] < b[i]) return true;
    if (a[i] > b[i]) return false;
  }
  return false;
}

std::string FindBestInput(absl::FunctionRef<bool(const std::vector<int>&, const std::vector<int>&)> better_input_p) {
  absl::flat_hash_map<int64_t, std::vector<int>> z_to_partial = {
    {0, {}},
  };
  for (int offset = 0; offset < 14; ++offset) {
    VLOG(1) << offset << ": " << z_to_partial.size();
    absl::flat_hash_map<int64_t, std::vector<int>> next_z_to_partial;
    for (const auto& [z, this_input] : z_to_partial) {
      std::vector<int> next_input = this_input;
      for (int input = 1; input <= 9; ++input) {
        int64_t next_z = DecompiledPartial(input, offset, z);
        next_input.push_back(input);
        auto [it, inserted] = next_z_to_partial.emplace(next_z, next_input);
        if (!inserted) {
          if (better_input_p(next_input, it->second)) {
            it->second = next_input;
          }
        }
        next_input.pop_back();
      }
    }
    z_to_partial = std::move(next_z_to_partial);
  }
  VLOG(1) << "Final: " << z_to_partial.size();
  VLOG(1) << "Contains 0 " << z_to_partial.contains(0);
  auto it = z_to_partial.find(0);
  CHECK(it != z_to_partial.end());
  VLOG(1) << absl::StrJoin(it->second, "");
  return absl::StrJoin(it->second, "");
}

}  // namespace

absl::StatusOr<std::string> Day_2021_24::Part1(
    absl::Span<absl::string_view> input) const {
  VM vm = VM::Parse(input);
  if (run_audit()) {
    std::vector<int> test = {1, 3, 5, 7, 9, 2, 4, 6, 8, 9, 9, 9, 9, 9};
    vm.Reset();
    if (auto st = vm.Execute(test); !st.ok()) return st;
    VLOG(1) << "Sample is " << vm.registers["z"];
  }
  if (run_audit()) {
    std::vector<int> test = {2, 9, 9, 8, 9, 2, 9, 7, 9, 4, 9, 5, 1, 9};
    vm.Reset();
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.registers["z"] != 0) return Error("Not 0 checksum");
  }
  return FindBestInput(BetterInputMax);

  std::vector<int> test(14, 9);
  int64_t best = std::numeric_limits<int64_t>::max();
  while (test[0] > 0) {
    vm.Reset();
    if (auto st = vm.Execute(test); !st.ok()) return st;
    int64_t z = vm.registers["z"];
    int64_t dec_z = Decompiled(test);
    if (dec_z != z) {
      VLOG(1) << " *** ERROR *** ";
      VLOG(1) << "Bad decompiled: " << dec_z << " != " << z << ": " << absl::StrJoin(test, "");
    }
    if (abs(best) > abs(z)) {
      VLOG(1) << absl::StrJoin(test, "") << ": " << z;
      best = z;
    }
    if (z == 0) {
      return absl::StrJoin(test, "");
    }
    for (int i = test.size() - 1; i >= 0; --i) {
      --test[i];
      if (test[i] != 0) break;
      if (i == 0) break;
      test[i] = 9;
    }
  }
  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2021_24::Part2(
    absl::Span<absl::string_view> input) const {
  VM vm = VM::Parse(input);
  if (run_audit()) {
    std::vector<int> test = {1, 9, 5, 1, 8, 1, 2, 1, 3, 1, 6, 1, 1, 8};
    vm.Reset();
    if (auto st = vm.Execute(test); !st.ok()) return st;
    if (vm.registers["z"] != 0) return Error("Not 0 checksum");
  }
  return FindBestInput(BetterInputMin);
}

}  // namespace advent_of_code
