#include "advent_of_code/2021/bits_expr.h"

#include "absl/strings/str_cat.h"
#include "glog/logging.h"

namespace advent_of_code {

absl::StatusOr<BitsExpr> BitsExpr::Parse(absl::string_view hex) {
  std::vector<bool> bits;
  for (const char c : hex) {
    int val = -1;
    if (c >= '0' && c <= '9')
      val = c - '0';
    else if (c >= 'A' && c <= 'F')
      val = c - 'A' + 10;
    else
      return absl::InvalidArgumentError("Bad hex");
    for (int bit = 3; bit >= 0; --bit) {
      bits.push_back(val & (1 << bit));
    }
  }
  // VLOG(1) << absl::StrJoin(bits, ",");
  return Parser(bits).ParseExpr();
}

absl::StatusOr<BitsExpr> BitsExpr::Parser::ParseExpr() {
  BitsExpr ret;
  absl::StatusOr<int64_t> ver = ParseInt(3);
  if (!ver.ok()) return ver.status();
  ret.version_ = *ver;
  absl::StatusOr<int64_t> type = ParseInt(3);
  if (!type.ok()) return type.status();
  ret.type_ = static_cast<EvaluateType>(*type);
  if (ret.type_ == EvaluateType::kLiteral) {
    ret.literal_ = 0;
    while (true) {
      absl::StatusOr<int64_t> more = ParseInt(1);
      if (!more.ok()) return more.status();
      absl::StatusOr<int64_t> n = ParseInt(4);
      if (!n.ok()) return n.status();
      ret.literal_ <<= 4;
      ret.literal_ += *n;
      if (!*more) break;
    }
    return ret;
  }

  absl::StatusOr<int64_t> l_type = ParseInt(1);
  if (!l_type.ok()) return l_type.status();

  if (*l_type == 0) {
    // 15-bit length;
    absl::StatusOr<int64_t> length = ParseInt(15);
    if (!length.ok()) return length.status();

    int64_t bit_end = offset_ + *length;
    while (offset_ < bit_end) {
      absl::StatusOr<BitsExpr> sub = ParseExpr();
      if (!sub.ok()) return sub.status();
      ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(*sub)));
    }
    return ret;
  }
  // 11-bit subpacket count;
  absl::StatusOr<int64_t> sub_count = ParseInt(11);
  if (!sub_count.ok()) return sub_count.status();
  for (int i = 0; i < *sub_count; ++i) {
    absl::StatusOr<BitsExpr> sub = ParseExpr();
    if (!sub.ok()) return sub.status();
    ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(*sub)));
  }
  return ret;
}

absl::StatusOr<int64_t> BitsExpr::Parser::ParseInt(int64_t count) {
  if (offset_ + count > bits_.size()) {
    return absl::InvalidArgumentError("Read past end of bits");
  }
  CHECK(offset_ < bits_.size());
  int64_t v = 0;
  for (int i = 0; i < count; ++i) {
    v = v * 2 + bits_[offset_ + i];
  }
  offset_ += count;
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
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret += *sv;
      }
      return ret;
    }
    case EvaluateType::kProduct: {
      int64_t ret = 1;
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret *= *sv;
      }
      return ret;
    }
    case EvaluateType::kMin: {
      int64_t ret = std::numeric_limits<int64_t>::max();
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret = std::min(ret, *sv);
      }
      return ret;
    }
    case EvaluateType::kMax: {
      int64_t ret = std::numeric_limits<int64_t>::min();
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret = std::max(ret, *sv);
      }
      return ret;
    }
    case EvaluateType::kLiteral: {
      return literal_;
    }
    case EvaluateType::kGreater: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError("Bad >");
      }
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left > *right;
    }
    case EvaluateType::kLess: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError("Bad <");
      }
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left < *right;
    }
    case EvaluateType::kEqual: {
      if (sub_.size() != 2) {
        return absl::InvalidArgumentError("Bad ==");
      }
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left == *right;
    }
    default: {
      return absl::InternalError(absl::StrCat("Bad type: ", type_));
    }
  }
  return absl::InternalError("Left fully covered switch");
}

absl::string_view BitsExpr::DebugString(BitsExpr::EvaluateType type) {
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
