#ifndef ADVENT_OF_CODE_2018_VM_H
#define ADVENT_OF_CODE_2018_VM_H

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "absl/log/log.h"
#include "advent_of_code/vlog.h"
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

    void Apply(std::vector<int32_t>& registers) const {
      switch (op_code) {
        case OpCode::kAddr: {
          registers[arg3] = registers[arg1] + registers[arg2];
          break;
        }
        case OpCode::kAddi: {
          registers[arg3] = registers[arg1] + arg2;
          break;
        }
        case OpCode::kMulr: {
          registers[arg3] = registers[arg1] * registers[arg2];
          break;
        }
        case OpCode::kMuli: {
          registers[arg3] = registers[arg1] * arg2;
          break;
        }
        case OpCode::kBanr: {
          registers[arg3] = registers[arg1] & registers[arg2];
          break;
        }
        case OpCode::kBani: {
          registers[arg3] = registers[arg1] & arg2;
          break;
        }
        case OpCode::kBorr: {
          registers[arg3] = registers[arg1] | registers[arg2];
          break;
        }
        case OpCode::kBori: {
          registers[arg3] = registers[arg1] | arg2;
          break;
        }
        case OpCode::kSetr: {
          registers[arg3] = registers[arg1];
          break;
        }
        case OpCode::kSeti: {
          registers[arg3] = arg1;
          break;
        }
        case OpCode::kGtir: {
          registers[arg3] = arg1 > registers[arg2] ? 1 : 0;
          break;
        }
        case OpCode::kGtri: {
          registers[arg3] = registers[arg1] > arg2 ? 1 : 0;
          break;
        }
        case OpCode::kGtrr: {
          registers[arg3] = registers[arg1] > registers[arg2] ? 1 : 0;
          break;
        }
        case OpCode::kEqir: {
          registers[arg3] = arg1 == registers[arg2] ? 1 : 0;
          break;
        }
        case OpCode::kEqri: {
          registers[arg3] = registers[arg1] == arg2 ? 1 : 0;
          break;
        }
        case OpCode::kEqrr: {
          registers[arg3] = registers[arg1] == registers[arg2] ? 1 : 0;
          break;
        }
      }
    }

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
      ops_[*ip].Apply(registers_);
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