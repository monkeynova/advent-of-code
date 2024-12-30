// http://adventofcode.com/2024/day/17

#include "advent_of_code/2024/day17/day17.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

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

std::optional<int64_t> RevEngA(absl::Span<const int64_t> prog, int64_t a) {
  if (prog.empty()) return a;

  int64_t next_val = prog.back();
  for (int64_t next = 0; next < 8; ++next) {
    int64_t test_a = (a << int64_t{3}) | next;
    int64_t test_b = next ^ 4;
    int64_t test_c = (test_a >> (next ^ 1)) % 8;
    if (next_val == (test_b ^ test_c)) {
      std::optional<int64_t> sub = RevEngA(prog.subspan(0, prog.size() - 1), test_a);
      if (sub) return sub;
    }
  }
  
  return std::nullopt;
}

std::optional<int64_t> RevEngA(absl::Span<const int64_t> prog) {
  // TODO: Extract from pattern: {doesn't touch a...} a /= 8; jnz
  const std::vector<int64_t> kMyInput = {2,4,1,1,7,5,1,5,4,0,5,5,0,3,3,0};
  if (prog != kMyInput) return std::nullopt;

  return RevEngA(prog, /*a=*/0);
}

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

  Program(std::array<int64_t, 3> registers, std::vector<int64_t> mem)
   : registers_(registers), mem_(std::move(mem)) {}

  std::vector<int64_t> Run() {
    while (!Step()) /*nop*/;
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
    //LOG(ERROR) << *this;

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

  int64_t* ComboOperand(int64_t* reg_id) {
    if (*reg_id <= 3) return reg_id;
    if (*reg_id <= 6) return &registers_[*reg_id - 4];
    LOG(FATAL) << ">=7 not supported";
  }

  int64_t ip_ = 0;
  std::array<int64_t, 3> registers_;
  std::vector<int64_t> mem_;
  std::vector<int64_t> out_;
};

// Helper methods go here.

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
    if (!t.Done()) return absl::InvalidArgumentError("bad input A");
  }
  {
    Tokenizer t(input[1]);
    RETURN_IF_ERROR(t.NextAre({"Register", "B", ":"}));
    ASSIGN_OR_RETURN(registers[1], t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad input B");
  }
  {
    Tokenizer t(input[2]);
    RETURN_IF_ERROR(t.NextAre({"Register", "C", ":"}));
    ASSIGN_OR_RETURN(registers[2], t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad input C");
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
    if (!t.Done()) return absl::InvalidArgumentError("bad input A");
  }
  {
    Tokenizer t(input[1]);
    RETURN_IF_ERROR(t.NextAre({"Register", "B", ":"}));
    ASSIGN_OR_RETURN(registers[1], t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad input B");
  }
  {
    Tokenizer t(input[2]);
    RETURN_IF_ERROR(t.NextAre({"Register", "C", ":"}));
    ASSIGN_OR_RETURN(registers[2], t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad input C");
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

  std::optional<int64_t> rev_a = RevEngA( mem);
  if (rev_a) {
    registers[0] = *rev_a;
    std::vector<int64_t> out = Program(registers, mem).Run();
    if (out != mem) {
      return absl::InternalError(absl::StrCat("Bad RevEngA: ", absl::StrJoin(out, ",")));
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
