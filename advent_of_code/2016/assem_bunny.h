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

    bool operator==(const Registers& o) const {
      return a == o.a && b == o.b && c == o.c && d == o.d;
    }

    friend std::ostream& operator<<(std::ostream& out,
                                    const AssemBunny::Registers& r) {
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
      absl::StatusOr<Instruction> i = Instruction::Parse(in, ret.registers_.get());
      if (!i.ok()) return i.status();
      ret.instructions_.push_back(std::move(*i));
    }
    return ret;
  }

  absl::Status Execute(OutputInterface* output_interface = nullptr,
                       PauseInterface* pause_interface = nullptr);

  Registers& registers() { return *registers_; }
  const Registers& registers() const { return *registers_; }

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
    int64_t Arg1() const {
      if (std::holds_alternative<int64_t*>(arg1)) {
        return *std::get<int64_t*>(arg1);
      } else if (std::holds_alternative<int64_t>(arg1)) {
        return std::get<int64_t>(arg1);
      } else {
        LOG(FATAL) << "Bad alternative: arg1";
      }
    }
    int64_t* MutableArg1() const {
      if (std::holds_alternative<int64_t*>(arg1)) {
        return std::get<int64_t*>(arg1);
      } else {
        LOG(FATAL) << "Not mutable: arg1";
      }
    }
    int64_t Arg2() const {
      if (std::holds_alternative<int64_t*>(arg2)) {
        return *std::get<int64_t*>(arg2);
      } else if (std::holds_alternative<int64_t>(arg2)) {
        return std::get<int64_t>(arg2);
      } else {
        LOG(FATAL) << "Bad alternative: arg2";
      }
    }
    int64_t* MutableArg2() const {
      if (std::holds_alternative<int64_t*>(arg2)) {
        return std::get<int64_t*>(arg2);
      } else {
        LOG(FATAL) << "Not mutable: arg2";
      }
    }

    static absl::StatusOr<Instruction> Parse(
      absl::string_view in, Registers* registers);

   private:
    std::variant<int64_t*, int64_t> arg1;
    std::variant<int64_t*, int64_t> arg2;
  };

  AssemBunny() : registers_(std::make_unique<Registers>()) {}

  std::vector<Instruction> instructions_;
  // We need pointer stability in registers wehn the VM is move'd.
  std::unique_ptr<Registers> registers_;
  int ip_ = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2016_ASSEM_BUNNY_H