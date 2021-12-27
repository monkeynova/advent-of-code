#ifndef ADVENT_OF_CODE_2021_DAY16_BITS_EXPR_H
#define ADVENT_OF_CODE_2021_DAY16_BITS_EXPR_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

namespace advent_of_code {

class BitsExpr {
 public:
  static absl::StatusOr<BitsExpr> Parse(absl::string_view hex);

  int64_t SumVersions() const;
  absl::StatusOr<int64_t> Evaluate() const;

  std::string DebugString() const {
    std::string ret;
    DebugStringImpl(&ret, "");
    return ret;
  }

  friend std::ostream& operator<<(std::ostream& out, const BitsExpr& b) {
    return out << b.DebugString();
  }

 private:
  enum class EvaluateType {
    kSum = 0,
    kProduct = 1,
    kMin = 2,
    kMax = 3,
    kLiteral = 4,
    kGreater = 5,
    kLess = 6,
    kEqual = 7,
  };

  class Parser {
   public:
    Parser(const std::vector<bool>& bits) : bits_(bits) {}

    absl::StatusOr<BitsExpr> ParseExpr();
    absl::StatusOr<int64_t> ReadInt(int64_t count);

   private:
    const std::vector<bool>& bits_;
    int64_t offset_ = 0;
  };

  static absl::string_view DebugString(BitsExpr::EvaluateType type);
  void DebugStringImpl(std::string* out, std::string prefix) const;

  int version_;
  EvaluateType type_;
  int64_t literal_;
  std::vector<std::unique_ptr<BitsExpr>> sub_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2021_DAY16_BITS_EXPR_H