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
                       PauseInterface* pause_interface = nullptr);

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
    static absl::StatusOr<Instruction> Parse(absl::string_view in);
  };

  AssemBunny() = default;

  std::vector<Instruction> instructions_;
  Registers registers_;
  int ip_ = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2016_ASSEM_BUNNY_H