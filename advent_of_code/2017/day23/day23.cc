// https://adventofcode.com/2017/day/23
//
// --- Day 23: Coprocessor Conflagration ---
// -----------------------------------------
// 
// You decide to head directly to the CPU and fix the printer from there.
// As you get close, you find an experimental coprocessor doing so much
// work that the local programs are afraid it will halt and catch fire.
// This would cause serious issues for the rest of the computer, so you
// head in and see what you can do.
// 
// The code it's running seems to be a variant of the kind you saw
// recently on that tablet. The general functionality seems very similar,
// but some of the instructions are different:
// 
// * set X Y sets register X to the value of Y.
// 
// * sub X Y decreases register X by the value of Y.
// 
// * mul X Y sets register X to the result of multiplying the value
// contained in register X by the value of Y.
// 
// * jnz X Y jumps with an offset of the value of Y, but only if the
// value of X is not zero. (An offset of 2 skips the next
// instruction, an offset of -1 jumps to the previous instruction,
// and so on.)
// 
// Only the instructions listed above are used. The eight registers
// here, named a through h, all start at 0.
// 
// The coprocessor is currently set to some kind of debug mode, which
// allows for testing, but prevents it from doing any meaningful work.
// 
// If you run the program (your puzzle input), how many times is the mul
// instruction invoked?
//
// --- Part Two ---
// ----------------
// 
// Now, it's time to fix the problem.
// 
// The debug mode switch is wired directly to register a. You flip the
// switch, which makes register a now start at 1 when the program is
// executed.
// 
// Immediately, the coprocessor begins to overheat. Whoever wrote this
// program obviously didn't choose a very efficient implementation.
// You'll need to optimize the program if it has any hope of completing
// before Santa needs that printer working.
// 
// The coprocessor's ultimate goal is to determine the final value left
// in register h once the program completes. Technically, if it had
// that... it wouldn't even need to run the program.
// 
// After setting register a to 1, if the program were to run to
// completion, what value would be left in register h?


#include "advent_of_code/2017/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

/*
Scratch space working with the input.

b = 65;
c = b;
if (a != 0) {
  b *= 100;
  b += 100000;  // 106500
  c = b;
  c += 17000    // 123500
  f = 1;        // 1
}
while (true) {
  for (d = 2; d != b; ++d) {
    for (e = 2; e != b; ++e) {
      if (d * e == b) f = 0;
    }
  }

  if (f == 0) ++h;
  if (b == c) break;
  b += 17;
}

set b 65
set c b
jnz a 2 {
  jnz 1 5 {
    mul b 100
    sub b -100000
    set c b
    sub c -17000
  }
}
{
  set f 1
  set d 2
  {
    set e 2
    {
      set g d
      mul g e
      sub g b
      jnz g 2 {
        set f 0
      }
      sub e -1
      set g e
      sub g b
      jnz g -8
    }
    sub d -1
    set g d
    sub g b
    jnz g -13
  }
  jnz f 2 {
    sub h -1
  }
  set g b
  sub g c
  jnz g 2 {
    jnz 1 3    // term
  }
  sub b -17
  jnz 1 -23
}

*/
namespace {

struct Instruction {
  enum {
    kSnd = 1,
    kSet = 2,
    kAdd = 3,
    kMul = 4,
    kMod = 5,
    kRcv = 6,
    kJgz = 7,
    kSub = 8,
    kJnz = 9,
  } op_code;
  absl::string_view arg1;
  absl::string_view arg2;

  static absl::StatusOr<Instruction> Parse(absl::string_view str) {
    Instruction ret;
    if (RE2::FullMatch(str, "snd (-?\\d+|[a-z])", &ret.arg1)) {
      ret.op_code = Instruction::kSnd;
    } else if (RE2::FullMatch(str, "set ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                              &ret.arg2)) {
      ret.op_code = Instruction::kSet;
    } else if (RE2::FullMatch(str, "sub ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                              &ret.arg2)) {
      ret.op_code = Instruction::kSub;
    } else if (RE2::FullMatch(str, "add ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                              &ret.arg2)) {
      ret.op_code = Instruction::kAdd;
    } else if (RE2::FullMatch(str, "mul ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                              &ret.arg2)) {
      ret.op_code = Instruction::kMul;
    } else if (RE2::FullMatch(str, "mod ([a-z]) (-?\\d+|[a-z])", &ret.arg1,
                              &ret.arg2)) {
      ret.op_code = Instruction::kMod;
    } else if (RE2::FullMatch(str, "rcv (-?\\d+|[a-z])", &ret.arg1)) {
      ret.op_code = Instruction::kRcv;
    } else if (RE2::FullMatch(str, "jgz (-?\\d+|[a-z]) (-?\\d+|[a-z])",
                              &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kJgz;
    } else if (RE2::FullMatch(str, "jnz (-?\\d+|[a-z]) (-?\\d+|[a-z])",
                              &ret.arg1, &ret.arg2)) {
      ret.op_code = Instruction::kJnz;
    } else {
      return Error("Bad instruction: ", str);
    }
    return ret;
  }
};

class VM {
 public:
  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input) {
    VM ret;
    for (absl::string_view ins : input) {
      absl::StatusOr<Instruction> next = Instruction::Parse(ins);
      if (!next.ok()) return next.status();
      ret.instructions_.push_back(*next);
    }
    return ret;
  }

  int get_register(absl::string_view r) { return registers_[r]; }

  void set_register(absl::string_view r, int id) { registers_[r] = id; }

  void ExecuteToRecv() {
    bool recv = false;
    while (!recv && ip_ < instructions_.size() && ip_ >= 0) {
      bool jumped = false;
      const Instruction& i = instructions_[ip_];
      switch (i.op_code) {
        case Instruction::kSnd: {
          send_queue_.push_back(GetValue(i.arg1));
          break;
        }
        case Instruction::kSet: {
          registers_[i.arg1] = GetValue(i.arg2);
          break;
        }
        case Instruction::kAdd: {
          registers_[i.arg1] += GetValue(i.arg2);
          break;
        }
        case Instruction::kSub: {
          registers_[i.arg1] -= GetValue(i.arg2);
          break;
        }
        case Instruction::kMul: {
          ++mul_count_;
          registers_[i.arg1] *= GetValue(i.arg2);
          break;
        }
        case Instruction::kMod: {
          registers_[i.arg1] %= GetValue(i.arg2);
          break;
        }
        case Instruction::kRcv: {
          if (!recv_queue_.empty()) {
            registers_[i.arg1] = recv_queue_.front();
            recv_queue_.pop_front();
          } else {
            if (!part1_ || GetValue(i.arg1) != 0) {
              recv = true;
              // Don't advance IP. Come back here.
              jumped = true;
            }
          }
          break;
        }
        case Instruction::kJgz: {
          if (GetValue(i.arg1) > 0) {
            ip_ += GetValue(i.arg2);
            jumped = true;
          }
          break;
        }
        case Instruction::kJnz: {
          if (GetValue(i.arg1) != 0) {
            ip_ += GetValue(i.arg2);
            jumped = true;
          }
          break;
        }
      }
      if (!jumped) ++ip_;
    }
  }

  bool done() { return done_; }

  std::vector<int> ConsumeSendQueue() {
    std::vector<int> ret = send_queue_;
    send_queue_.clear();
    return ret;
  }

  void AddRecvQueue(std::vector<int> in) {
    recv_queue_.insert(recv_queue_.end(), in.begin(), in.end());
  }

  void set_part1() { part1_ = true; }
  int mul_count() const { return mul_count_; }

 private:
  VM() {
    absl::string_view reg_names = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < reg_names.size(); ++i) {
      registers_[reg_names.substr(i, 1)] = 0;
    }
  }

  int GetValue(absl::string_view name) const {
    if (auto it = registers_.find(name); it != registers_.end())
      return it->second;
    int n;
    CHECK(absl::SimpleAtoi(name, &n));
    return n;
  }

  std::vector<Instruction> instructions_;
  int ip_ = 0;
  absl::flat_hash_map<std::string_view, int64_t> registers_;
  std::vector<int> send_queue_;
  std::deque<int> recv_queue_;
  bool done_ = false;
  bool part1_ = false;
  int mul_count_ = 0;
};

}  // namespace

absl::StatusOr<std::string> Day_2017_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();

  vm->ExecuteToRecv();

  return IntReturn(vm->mul_count());
}

absl::StatusOr<std::string> Day_2017_23::Part2(
    absl::Span<absl::string_view> input) const {
  // Hand converted, then optimized form of the assembly.
  // TODO(@monkeynova): Can we identify and optimize this?
  int b = 65;
  int c = b;
  b *= 100;
  b += 100000;
  c = b;
  c += 17000;
  int h = 0;
  for (; true; b += 17) {
    bool f = true;
    for (int d = 2; d != b; ++d) {
      if (b % d == 0) f = false;
      /*
      for (e = 2; e != b; ++e) {
        if (d * e == b) f = 0;
      }
      */
    }
    if (!f) ++h;
    if (b == c) break;
  }
  return IntReturn(h);
}

}  // namespace advent_of_code
