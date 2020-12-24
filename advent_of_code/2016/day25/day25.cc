#include "advent_of_code/2016/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

enum class OpCode {
  kCpy = 1,
  kInc = 2,
  kDec = 3,
  kJnz = 4,
  kTgl = 5,
  kOut = 6,
};

struct Instruction {
  OpCode op_code;
  absl::string_view arg1;
  absl::string_view arg2;

  static absl::StatusOr<Instruction> Parse(absl::string_view in) {
    Instruction i;
    if (RE2::FullMatch(in, "cpy ([-a-z0-9]+) ([a-z]+)", &i.arg1, &i.arg2)) {
      i.op_code = OpCode::kCpy;
    } else if (RE2::FullMatch(in, "inc ([a-z]+)", &i.arg1)) {
      i.op_code = OpCode::kInc;
    } else if (RE2::FullMatch(in, "dec ([a-z]+)", &i.arg1)) {
      i.op_code = OpCode::kDec;
    } else if (RE2::FullMatch(in, "jnz ([-a-z0-9]+) ([-a-z0-9]+)", &i.arg1,
                              &i.arg2)) {
      i.op_code = OpCode::kJnz;
    } else if (RE2::FullMatch(in, "tgl ([-a-z0-9]+)", &i.arg1)) {
      i.op_code = OpCode::kTgl;
    } else if (RE2::FullMatch(in, "out ([-a-z0-9]+)", &i.arg1)) {
      i.op_code = OpCode::kOut;
    } else {
      return AdventDay::Error("Bad instruction: ", in);
    }
    return i;
  }
};

struct Registers {
  int64_t a = 0;
  int64_t b = 0;
  int64_t c = 0;
  int64_t d = 0;
  int64_t* Val(absl::string_view name) {
    static int64_t literal;
    if (name == "a") return &a;
    if (name == "b") return &b;
    if (name == "c") return &c;
    if (name == "d") return &d;
    if (absl::SimpleAtoi(name, &literal)) return &literal;
    return nullptr;
  }

  bool operator==(const Registers& o) const {
    return a == o.a && b == o.b && c == o.c && d == o.d;
  }
};

std::ostream& operator<<(std::ostream& out, const Registers& r) {
  return out << "{" << r.a << "," << r.b << "," << r.c << "," << r.d << "}";
}

template <typename H>
H AbslHashValue(H h, const Registers& r) {
  return H::combine(std::move(h), r.a, r.b, r.c, r.d);
}

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view in : input) {
      absl::StatusOr<Instruction> i = Instruction::Parse(in);
      if (!i.ok()) return i.status();
      ret.instructions_.push_back(std::move(*i));
    }
    return ret;
  }

  absl::Status Execute() {
    int ip = 0;
    absl::flat_hash_set<Registers> output_history;
    while (ip < instructions_.size()) {
      if (ip < 0) return AdventDay::Error("Bad ip: ", ip);
      const Instruction& i = instructions_[ip];
      bool jumped = false;
      switch (i.op_code) {
        case OpCode::kCpy: {
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          int64_t* output = registers_.Val(i.arg2);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
          *output = *input;
          break;
        }
        case OpCode::kInc: {
          int64_t* output = registers_.Val(i.arg1);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          ++*output;
          break;
        }
        case OpCode::kDec: {
          int64_t* output = registers_.Val(i.arg1);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          --*output;
          break;
        }
        case OpCode::kJnz: {
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          int64_t* dest = registers_.Val(i.arg2);
          if (dest == nullptr) return AdventDay::Error("Bad arg: ", i.arg2);
          if (*input != 0) {
            ip += *dest;
            jumped = true;
          }
          break;
        }
        case OpCode::kTgl: {
          return AdventDay::Error("TGL not supported");
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          if (*input + ip >= 0 && *input + ip < instructions_.size()) {
            Instruction& to_edit = instructions_[*input + ip];
            switch (to_edit.op_code) {
              // One-arg ops.
              case OpCode::kInc:
                to_edit.op_code = OpCode::kDec;
                break;
              case OpCode::kDec:
                to_edit.op_code = OpCode::kInc;
                break;
              case OpCode::kTgl:
                to_edit.op_code = OpCode::kInc;
                break;
              // Two arg ops.
              case OpCode::kJnz:
                to_edit.op_code = OpCode::kCpy;
                break;
              case OpCode::kCpy:
                to_edit.op_code = OpCode::kJnz;
                break;
              default:
                return AdventDay::Error("Bad op to toggle");
            }
          }
          break;
        }
        case OpCode::kOut: {
          int64_t* output = registers_.Val(i.arg1);
          if (output == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          VLOG(2) << "Output: " << *output << registers_;
          if (output_history.contains(registers_)) {
            return absl::OkStatus();
          }
          output_.push_back(*output);
          output_history.insert(registers_);
          break;
        }
      }
      if (!jumped) ++ip;
    }
    return absl::OkStatus();
  }

  const std::vector<int>& output() { return output_; }

  int64_t register_a() { return registers_.a; }
  void set_register_a(int64_t v) { registers_.a = v; }

 private:
  VM() = default;

  std::vector<Instruction> instructions_;
  Registers registers_;
  std::vector<int> output_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day25_2016::Part1(
    absl::Span<absl::string_view> input) const {
  if (false) {
    for (int64_t a = 0; true; ++a) {
      int64_t foo = a + 4 * 643;
      bool matches = true;
      while (matches && foo) {
        if ((foo & 0x3) != 0x2) {
          matches = false;
        }
        foo >>= 2;
      }
      if (matches) return IntReturn(a);
    }
  }
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  for (int a = 0; true; ++a) {
    VM copy = *vm;
    VLOG(1) << "Trying: " << a;
    copy.set_register_a(a);
    if (absl::Status st = copy.Execute(); !st.ok()) return st;
    bool check = true;
    const std::vector<int>& output = copy.output();
    VLOG(1) << " output: " << absl::StrJoin(output, ",");
    for (int i = 0; check && i < output.size(); ++i) {
      if (output[i] != 0 && output[i] != 1) check = false;
      if (output[i] == output[(i + 1) % output.size()]) check = false;
    }
    if (check) {
      return IntReturn(a);
    }
  }
  return Error("Impossible");
}

absl::StatusOr<std::vector<std::string>> Day25_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return std::vector<std::string>{"Merry Christmas"};
}

}  // namespace advent_of_code
