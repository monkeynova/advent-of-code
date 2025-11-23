// http://adventofcode.com/2024/day/17

#include "advent_of_code/2024/day17/day17.h"

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

/*

 BST [a]
 BXL  1
 CDV [b]
 BXL  5
 BXC  0
 OUT [b]
 ADV  3
 JNZ  0

 b = 0
 c = 0
 while (a != 0) {
   b = a % 8
   b ^= 1
   c = a >> b
   b ^= 5
   b ^= c
   out b % 8
   a >>= 3
 }

 b = 0
 c = 0
 while (a != 0) {
   out (a >> ((a % 8) ^ 1)) ^ ((a % 8) ^ 4) % 8
   a >>= 3
 }

*/

class Program {
 public:
  enum OpCode {
    kAdv = 0,
    kBxl = 1,
    kBst = 2,
    kJnz = 3,
    kBxc = 4,
    kOut = 5,
    kBdv = 6,
    kCdv = 7,
  };

  Program(std::array<int64_t, 3> registers, absl::Span<const int64_t> mem)
      : registers_(registers), mem_(mem) {}

  void Reset(std::array<int64_t, 3> registers) {
    registers_ = registers;
    ip_ = 0;
    out_ = {};
  }

  std::vector<int64_t> Run() {
    while (!Step()) /*nop*/
      ;
    return out_;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Program& p) {
    absl::Format(&sink, "ip=%d; ", p.ip_);
    absl::Format(&sink, "registers={%s}; ", absl::StrJoin(p.registers_, ","));
    absl::Format(&sink, "mem={%s}", absl::StrJoin(p.mem_, ","));
  }

 private:
  bool Step() {
    // LOG(ERROR) << *this;

    if (ip_ >= mem_.size()) return true;
    switch (mem_[ip_]) {
      case kAdv: {
        registers_[0] >>= *ComboOperand(&mem_[ip_ + 1]);
        break;
      }
      case kBxl: {
        registers_[1] ^= mem_[ip_ + 1];
        break;
      }
      case kBst: {
        registers_[1] = *ComboOperand(&mem_[ip_ + 1]) % 8;
        break;
      }
      case kJnz: {
        if (registers_[0] != 0) {
          ip_ = mem_[ip_ + 1];
          return false;
        }
        break;
      }
      case kBxc: {
        registers_[1] ^= registers_[2];
        break;
      }
      case kOut: {
        out_.push_back(*ComboOperand(&mem_[ip_ + 1]) % 8);
        break;
      }
      case kBdv: {
        registers_[1] = registers_[0] >> *ComboOperand(&mem_[ip_ + 1]);
        break;
      }
      case kCdv: {
        registers_[2] = registers_[0] >> *ComboOperand(&mem_[ip_ + 1]);
        break;
      }
    }
    ip_ += 2;
    return false;
  }

  const int64_t* ComboOperand(const int64_t* reg_id) {
    if (*reg_id <= 3) return reg_id;
    if (*reg_id <= 6) return &registers_[*reg_id - 4];
    LOG(FATAL) << ">=7 not supported";
  }

  int64_t ip_ = 0;
  std::array<int64_t, 3> registers_;
  absl::Span<const int64_t> mem_;
  std::vector<int64_t> out_;
};

std::optional<int64_t> Unwind(Program& p, absl::Span<const int64_t> output,
                              int64_t a) {
  if (output.empty()) return a;

  for (int64_t next = 0; next < 8; ++next) {
    int64_t test_a = (a << int64_t{3}) | next;
    std::array<int64_t, 3> registers = {test_a, 0, 0};
    p.Reset(registers);
    std::vector<int64_t> out = p.Run();
    if (out.size() <= output.size() &&
        out == output.subspan(output.size() - out.size(), out.size())) {
      std::optional<int64_t> sub =
          Unwind(p, output.subspan(0, output.size() - out.size()), test_a);
      if (sub) return sub;
    }
  }

  return std::nullopt;
}

std::optional<int64_t> RevEngA(absl::Span<const int64_t> prog) {
  // Last instruction must be [Jnz 0].
  if (prog[prog.size() - 2] != 3) return std::nullopt;
  if (prog[prog.size() - 1] != 0) return std::nullopt;

  // Must be no other jumps.
  for (int i = 0; i < prog.size() - 2; i += 2) {
    if (prog[i] == 3) return std::nullopt;
  }

  // Must be exactly one alteration of a, and must be >>= 3.
  int a_change_count = 0;
  for (int i = 0; i < prog.size() - 2; i += 2) {
    if (prog[i] == 0) {
      ++a_change_count;
      if (prog[i + 1] != 3) return std::nullopt;
    }
    // Must be no other jumps.
    if (prog[i] == 3) return std::nullopt;
  }
  if (a_change_count != 1) return std::nullopt;

  Program p({0, 0, 0}, prog.subspan(0, prog.size() - 2));
  return Unwind(p, prog, 0);
}

}  // namespace

absl::StatusOr<std::string> Day_2024_17::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 5) return absl::InvalidArgumentError("bad input (size)");
  if (input[3] != "") return absl::InvalidArgumentError("bad input (empty)");

  std::array<int64_t, 3> registers;
  {
    Tokenizer t(input[0]);
    RETURN_IF_ERROR(t.NextAre({"Register", "A", ":"}));
    ASSIGN_OR_RETURN(registers[0], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  {
    Tokenizer t(input[1]);
    RETURN_IF_ERROR(t.NextAre({"Register", "B", ":"}));
    ASSIGN_OR_RETURN(registers[1], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  {
    Tokenizer t(input[2]);
    RETURN_IF_ERROR(t.NextAre({"Register", "C", ":"}));
    ASSIGN_OR_RETURN(registers[2], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  std::vector<int64_t> mem;
  {
    Tokenizer t(input[4]);
    RETURN_IF_ERROR(t.NextAre({"Program", ":"}));
    for (bool first = true; !t.Done(); first = false) {
      if (!first) RETURN_IF_ERROR(t.NextIs(","));
      ASSIGN_OR_RETURN(int64_t v, t.NextInt());
      mem.push_back(v);
    }
  }

  Program p(registers, std::move(mem));
  return AdventReturn(absl::StrJoin(p.Run(), ","));
}

absl::StatusOr<std::string> Day_2024_17::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 5) return absl::InvalidArgumentError("bad input (size)");
  if (input[3] != "") return absl::InvalidArgumentError("bad input (empty)");

  std::array<int64_t, 3> registers;
  {
    Tokenizer t(input[0]);
    RETURN_IF_ERROR(t.NextAre({"Register", "A", ":"}));
    ASSIGN_OR_RETURN(registers[0], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  {
    Tokenizer t(input[1]);
    RETURN_IF_ERROR(t.NextAre({"Register", "B", ":"}));
    ASSIGN_OR_RETURN(registers[1], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  {
    Tokenizer t(input[2]);
    RETURN_IF_ERROR(t.NextAre({"Register", "C", ":"}));
    ASSIGN_OR_RETURN(registers[2], t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  std::vector<int64_t> mem;
  {
    Tokenizer t(input[4]);
    RETURN_IF_ERROR(t.NextAre({"Program", ":"}));
    for (bool first = true; !t.Done(); first = false) {
      if (!first) RETURN_IF_ERROR(t.NextIs(","));
      ASSIGN_OR_RETURN(int64_t v, t.NextInt());
      mem.push_back(v);
    }
  }

  std::optional<int64_t> rev_a = RevEngA(mem);
  if (rev_a) {
    registers[0] = *rev_a;
    std::vector<int64_t> out = Program(registers, mem).Run();
    if (out != mem) {
      return absl::InternalError(
          absl::StrCat("Bad RevEngA: ", absl::StrJoin(out, ",")));
    }
    return AdventReturn(*rev_a);
  }

  for (int64_t a = 0; true; ++a) {
    registers[0] = a;
    std::vector<int64_t> out = Program(registers, mem).Run();
    if (a % 777'777 == 0) {
      LOG(ERROR) << a << ": " << absl::StrJoin(out, ",");
    }
    if (out == mem) {
      return AdventReturn(a);
    }
  }
  LOG(FATAL) << "Left infinite loop";
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/17,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_17()); });

}  // namespace advent_of_code
