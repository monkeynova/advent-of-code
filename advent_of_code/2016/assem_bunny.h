#ifndef ADVENT_OF_CODE_2016_ASSEM_BUNNY_H
#define ADVENT_OF_CODE_2016_ASSEM_BUNNY_H

#include <cstdint>
#include <iostream>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "advent_of_code/advent_day.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

class AssemBunny {
 public:
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

    friend std::ostream& operator<<(std::ostream& out, const AssemBunny::Registers& r) {
      return out << "{" << r.a << "," << r.b << "," << r.c << "," << r.d << "}";
    }

    template <typename H>
    friend H AbslHashValue(H h, const AssemBunny::Registers& r) {
      return H::combine(std::move(h), r.a, r.b, r.c, r.d);
    }
  };

  class OutputInterface {
   public:
    virtual ~OutputInterface() = default;
    virtual absl::Status OnOutput(int64_t v, const AssemBunny* ab) = 0;
  };

  class PauseInterface {
   public:
    virtual ~PauseInterface() = default;
    virtual bool Pause() = 0;
  };

  static absl::StatusOr<AssemBunny> Parse(absl::Span<absl::string_view> input) {
    AssemBunny ret;
    for (absl::string_view in : input) {
      absl::StatusOr<Instruction> i = Instruction::Parse(in);
      if (!i.ok()) return i.status();
      ret.instructions_.push_back(std::move(*i));
    }
    return ret;
  }

  absl::Status Execute(OutputInterface* output_interface = nullptr,
                       PauseInterface* pause_interface = nullptr) {
    while (ip_ < instructions_.size()) {
      if (pause_interface != nullptr && pause_interface->Pause()) break;
      if (ip_ < 0) return AdventDay::Error("Bad ip: ", ip_);
      const Instruction& i = instructions_[ip_];
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
            ip_ += *dest;
            jumped = true;
          }
          break;
        }
        case OpCode::kTgl: {
          int64_t* input = registers_.Val(i.arg1);
          if (input == nullptr) return AdventDay::Error("Bad arg: ", i.arg1);
          if (*input + ip_ >= 0 && *input + ip_ < instructions_.size()) {
            Instruction& to_edit = instructions_[*input + ip_];
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
          if (output_interface == nullptr) return AdventDay::Error("No output sink");
          if (absl::Status st = output_interface->OnOutput(*output, this); !st.ok()) return st;
          break;
        }
      }
      if (!jumped) ++ip_;
    }
    return absl::OkStatus();
  }

  Registers& registers() { return registers_; }
  const Registers& registers() const { return registers_; }

 private:
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

  AssemBunny() = default;

  std::vector<Instruction> instructions_;
  Registers registers_;
  int ip_ = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2016_ASSEM_BUNNY_H