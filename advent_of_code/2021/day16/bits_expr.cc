#include "advent_of_code/2021/day16/bits_expr.h"

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "vlog.h"

namespace advent_of_code {

absl::StatusOr<BitsExpr> BitsExpr::Parse(std::string_view hex) {
  std::vector<bool> bits;
  for (const char c : hex) {
    int nibble = -1;
    if (c >= '0' && c <= '9') {
      nibble = c - '0';
    } else if (c >= 'A' && c <= 'F') {
      nibble = c - 'A' + 10;
    } else {
      return absl::InvalidArgumentError(
          absl::StrCat("Bad hex: ", std::string_view(&c, 1)));
    }
    for (int bit = 3; bit >= 0; --bit) {
      bits.push_back(nibble & (1 << bit));
    }
  }
  return Parser(bits).ParseExpr();
}

absl::StatusOr<BitsExpr> BitsExpr::Parser::ParseExpr() {
  BitsExpr ret;
  ASSIGN_OR_RETURN(ret.version_, ReadInt(3));
  ASSIGN_OR_RETURN(int64_t type, ReadInt(3));
  ret.type_ = static_cast<EvaluateType>(type);

  if (ret.type_ == EvaluateType::kLiteral) {
    ret.literal_ = 0;
    int64_t more;
    do {
      ASSIGN_OR_RETURN(more, ReadInt(1));
      ASSIGN_OR_RETURN(int64_t nibble, ReadInt(4));
      ret.literal_ = (ret.literal_ << 4) | nibble;
    } while (more);
    return ret;
  }

  ASSIGN_OR_RETURN(int64_t l_type, ReadInt(1));

  if (l_type == 0) {
    // 15-bit length in which sub-records exist.
    ASSIGN_OR_RETURN(int64_t length, ReadInt(15));

    int64_t bit_end = offset_ + length;
    while (offset_ < bit_end) {
      ASSIGN_OR_RETURN(BitsExpr sub, ParseExpr());
      ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(sub)));
    }
    if (offset_ != bit_end) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Submessages not at length: ", offset_, " != ", bit_end));
    }
    return ret;
  }

  // 11-bit sub-record count.
  ASSIGN_OR_RETURN(int64_t sub_count, ReadInt(11));
  for (int i = 0; i < sub_count; ++i) {
    ASSIGN_OR_RETURN(BitsExpr sub, ParseExpr());
    ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(sub)));
  }
  return ret;
}

absl::StatusOr<int64_t> BitsExpr::Parser::ReadInt(int64_t count) {
  if (offset_ + count > bits_.size()) {
    return absl::InvalidArgumentError("Read past end of bits");
  }
  int64_t v = 0;
  for (int i = 0; i < count; ++i) {
    v = (v << 1) | bits_[offset_++];
  }
  return v;
}

int64_t BitsExpr::SumVersions() const {
  int64_t ret = version_;
  for (const auto& sub : sub_) {
    ret += sub->SumVersions();
  }
  return ret;
}

absl::StatusOr<int64_t> BitsExpr::Evaluate() const {
  switch (type_) {
    case EvaluateType::kSum: {
      int64_t ret = 0;
      for (const auto& sub : sub_) {
        ASSIGN_OR_RETURN(int64_t sv, sub->Evaluate());
        ret += sv;
      }
      return ret;
    }
    case EvaluateType::kProduct: {
      int64_t ret = 1;
      for (const auto& sub : sub_) {
        ASSIGN_OR_RETURN(int64_t sv, sub->Evaluate());
        ret *= sv;
      }
      return ret;
    }
    case EvaluateType::kMin: {
      int64_t ret = std::numeric_limits<int64_t>::max();
      for (const auto& sub : sub_) {
        ASSIGN_OR_RETURN(int64_t sv, sub->Evaluate());
        ret = std::min(ret, sv);
      }
      return ret;
    }
    case EvaluateType::kMax: {
      int64_t ret = std::numeric_limits<int64_t>::min();
      for (const auto& sub : sub_) {
        ASSIGN_OR_RETURN(int64_t sv, sub->Evaluate());
        ret = std::max(ret, sv);
      }
      return ret;
    }
    case EvaluateType::kLiteral: {
      return literal_;
    }
    case EvaluateType::kGreater: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError(
            absl::StrCat("Invalid argument count for Greater: ", sub_.size()));
      }
      ASSIGN_OR_RETURN(int64_t left, sub_[0]->Evaluate());
      ASSIGN_OR_RETURN(int64_t right, sub_[1]->Evaluate());
      return left > right;
    }
    case EvaluateType::kLess: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError(
            absl::StrCat("Invalid argument count for Less: ", sub_.size()));
      }
      ASSIGN_OR_RETURN(int64_t left, sub_[0]->Evaluate());
      ASSIGN_OR_RETURN(int64_t right, sub_[1]->Evaluate());
      return left < right;
    }
    case EvaluateType::kEqual: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError(
            absl::StrCat("Invalid argument count for Equal: ", sub_.size()));
      }
      ASSIGN_OR_RETURN(int64_t left, sub_[0]->Evaluate());
      ASSIGN_OR_RETURN(int64_t right, sub_[1]->Evaluate());
      return left == right;
    }
    default: {
      return absl::InternalError(absl::StrCat("Bad type: ", type_));
    }
  }
  return absl::InternalError("Left fully covered switch");
}

std::string_view BitsExpr::DebugString(BitsExpr::EvaluateType type) {
  switch (type) {
    case EvaluateType::kSum:
      return "Sum";
    case EvaluateType::kProduct:
      return "Product";
    case EvaluateType::kMin:
      return "Min";
    case EvaluateType::kMax:
      return "Max";
    case EvaluateType::kLiteral:
      return "Literal";
    case EvaluateType::kGreater:
      return "Greater";
    case EvaluateType::kLess:
      return "Less";
    case EvaluateType::kEqual:
      return "Equal";
  }
  return "Bad type";
}

void BitsExpr::DebugStringImpl(std::string* out, std::string prefix) const {
  absl::StrAppend(out, prefix, "+-ver=", version_, "\n");
  absl::StrAppend(out, prefix, "+-type=", DebugString(type_), "\n");
  if (type_ == EvaluateType::kLiteral) {
    absl::StrAppend(out, prefix, "  +-literal=", literal_, "\n");
  } else {
    prefix += "  ";
    for (const auto& sub : sub_) {
      sub->DebugStringImpl(out, prefix);
    }
  }
}

}  // namespace advent_of_code
