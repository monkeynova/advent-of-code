#include "advent_of_code/2017/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

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

  void set_program_id(int id) { registers_["p"] = id; }

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
        case Instruction::kMul: {
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
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->set_part1();
  vm->ExecuteToRecv();
  std::vector<int> send_queue = vm->ConsumeSendQueue();
  return AdventReturn(send_queue.back());
}

absl::StatusOr<std::string> Day_2017_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<VM> vm = VM::Parse(input);
  if (!vm.ok()) return vm.status();
  VM p0 = *vm;
  p0.set_program_id(0);
  VM p1 = *vm;
  p1.set_program_id(1);
  bool saw_send = true;
  int p1_sends = 0;
  while (saw_send) {
    saw_send = false;
    p0.ExecuteToRecv();
    std::vector<int> send_queue = p0.ConsumeSendQueue();
    VLOG(1) << "p0 send_queue: " << send_queue.size();
    saw_send |= !send_queue.empty();
    p1.AddRecvQueue(send_queue);
    p1.ExecuteToRecv();
    send_queue = p1.ConsumeSendQueue();
    VLOG(1) << "p1 send_queue: " << send_queue.size();
    p1_sends += send_queue.size();
    saw_send |= !send_queue.empty();
    p0.AddRecvQueue(send_queue);
  }
  return AdventReturn(p1_sends);
}

}  // namespace advent_of_code
