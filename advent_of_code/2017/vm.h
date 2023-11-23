#ifndef ADVENT_OF_CODE_2017_VM_H
#define ADVENT_OF_CODE_2017_VM_H

#include <string_view>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/log/check.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "re2/re2.h"

namespace advent_of_code {

class VM_2017 {
 public:
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
    std::string_view arg1;
    std::string_view arg2;

    static absl::StatusOr<Instruction> Parse(std::string_view str);
  };

  static absl::StatusOr<VM_2017> Parse(absl::Span<std::string_view> input);

  int get_register(std::string_view r) { return registers_[r]; }
  void set_register(std::string_view r, int id) { registers_[r] = id; }

  void ExecuteToRecv();

  bool done() { return done_; }

  std::vector<int> ConsumeSendQueue() {
    std::vector<int> ret = send_queue_;
    send_queue_.clear();
    return ret;
  }

  void AddRecvQueue(std::vector<int> in) {
    recv_queue_.insert(recv_queue_.end(), in.begin(), in.end());
  }

  int mul_count() const { return mul_count_; }

 private:
  VM_2017() {
    std::string_view reg_names = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < reg_names.size(); ++i) {
      registers_[reg_names.substr(i, 1)] = 0;
    }
  }

  int GetValue(std::string_view name) const {
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
  int mul_count_ = 0;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2017_VM_H