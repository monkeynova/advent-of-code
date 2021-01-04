#ifndef ADVENT_OF_CODE_2018_VM_H
#define ADVENT_OF_CODE_2018_VM_H

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

class VM {
 public:
  enum OpCode {
    kAddr = 1,
    kAddi = 2,
    kMulr = 3,
    kMuli = 4,
    kBanr = 5,
    kBani = 6,
    kBorr = 7,
    kBori = 8,
    kSetr = 9,
    kSeti = 10,
    kGtir = 11,
    kGtri = 12,
    kGtrr = 13,
    kEqir = 14,
    kEqri = 15,
    kEqrr = 16,
  };

  struct Op {
    static const absl::flat_hash_map<absl::string_view, OpCode> kNameMap;
    static const std::vector<absl::string_view> kCodeMap;

    OpCode op_code;
    int32_t arg1;
    int32_t arg2;
    int32_t arg3;

    static absl::StatusOr<Op> Parse(absl::string_view str);

    absl::Status Apply(std::vector<int32_t>& registers) const;

    friend std::ostream& operator<<(std::ostream& out, const Op& op) {
      return out << kCodeMap[op.op_code] << ": " << op.arg1 << ", " << op.arg2
                 << ", " << op.arg3;
    }
  };

  static absl::StatusOr<VM> Parse(absl::Span<absl::string_view> input);

  absl::Status Execute() {
    return Execute([](int) { return false; });
  }

  absl::Status Execute(std::function<bool(int)> watcher) {
    int32_t* ip = &registers_[ip_register_];
    while (*ip >= 0 && *ip < ops_.size()) {
      if (watcher(*ip)) break;
      VLOG(4) << *ip << ": " << ops_[*ip] << "; "
              << absl::StrJoin(registers_, ",");
      if (absl::Status st = ops_[*ip].Apply(registers_); !st.ok()) {
        return st;
      }
      ++*ip;
    }
    return absl::OkStatus();
  }

  int32_t register_value(int register_number) const {
    return registers_[register_number];
  }

  void set_register_value(int register_number, int32_t value) {
    registers_[register_number] = value;
  }

  const std::vector<Op>& ops() const { return ops_; }

 private:
  VM() : registers_(6, 0) {}

  std::vector<Op> ops_;
  int ip_register_;
  std::vector<int32_t> registers_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2018_VM_H