#ifndef ADVENT_OF_CODE_2019_INT_CODE_H
#define ADVENT_OF_CODE_2019_INT_CODE_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

class IntCode {
 public:
  static absl::StatusOr<IntCode> Parse(
    const std::vector<absl::string_view>& input);

  IntCode(const IntCode&) = delete;
  IntCode& operator=(const IntCode&) = delete;
  IntCode(IntCode&&) = default;
  IntCode& operator=(IntCode&&) = default;

  IntCode Clone() const { return IntCode(codes_, code_pos_); }

  absl::Status Poke(int address, int value) {
     if (address < 0 || address >= codes_.size()) {
        return absl::InvalidArgumentError("Bad address");
     }
     codes_[address] = value;
     return absl::OkStatus();
  }

  absl::StatusOr<int> Peek(int address) {
     if (address < 0 || address >= codes_.size()) {
        return absl::InvalidArgumentError("Bad address");
     }
     return codes_[address];
  }

  absl::Status Run(const std::vector<int>& input = {}, std::vector<int>* output = nullptr);

 private:
  IntCode(std::vector<int> codes, int code_pos = 0)
   : codes_(std::move(codes)),
     code_pos_(code_pos) {}

  absl::StatusOr<int> LoadParameter(int parameter_modes, int parameter) const;
  absl::Status SaveParameter(int parameter_modes, int parameter, int value);

  std::vector<int> codes_;
  int code_pos_;
};

#endif  // ADVENT_OF_CODE_2019_INT_CODE_H