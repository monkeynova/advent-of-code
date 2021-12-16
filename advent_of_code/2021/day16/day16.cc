#include "advent_of_code/2021/day16/day16.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class BitsExpr {
 public:
  static absl::StatusOr<BitsExpr> Parse(absl::string_view hex);

  int64_t SumVersions() const;
  absl::StatusOr<int64_t> Evaluate() const;

 private:
  static absl::StatusOr<BitsExpr> ParseExpr(const std::vector<bool>& bits,
                                            int64_t& offset);
  static absl::StatusOr<int64_t> ParseInt(const std::vector<bool>& bits,
                                          int64_t& offset, int64_t count);

  int version_;
  int type_;
  int64_t literal_;
  std::vector<std::unique_ptr<BitsExpr>> sub_;

  friend class BitsExprParser;
};

absl::StatusOr<BitsExpr> BitsExpr::Parse(absl::string_view hex) {
  std::vector<bool> bits;
  for (const char c : hex) {
    int val = -1;
    if (c >= '0' && c <= '9')
      val = c - '0';
    else if (c >= 'A' && c <= 'F')
      val = c - 'A' + 10;
    else
      return Error("Bad hex");
    for (int bit = 3; bit >= 0; --bit) {
      bits.push_back(val & (1 << bit));
    }
  }
  // VLOG(1) << absl::StrJoin(bits, ",");
  int64_t offset = 0;
  return ParseExpr(bits, offset);
}

absl::StatusOr<BitsExpr> BitsExpr::ParseExpr(const std::vector<bool>& bits,
                                             int64_t& offset) {
  BitsExpr ret;
  absl::StatusOr<int64_t> ver = ParseInt(bits, offset, 3);
  if (!ver.ok()) return ver.status();
  ret.version_ = *ver;
  absl::StatusOr<int64_t> type = ParseInt(bits, offset, 3);
  if (!type.ok()) return type.status();
  ret.type_ = *type;
  if (ret.type_ == 4) {
    ret.literal_ = 0;
    while (true) {
      absl::StatusOr<int64_t> more = ParseInt(bits, offset, 1);
      if (!more.ok()) return more.status();
      absl::StatusOr<int64_t> n = ParseInt(bits, offset, 4);
      if (!n.ok()) return n.status();
      ret.literal_ <<= 4;
      ret.literal_ += *n;
      if (!*more) break;
    }
    return ret;
  }

  absl::StatusOr<int64_t> l_type = ParseInt(bits, offset, 1);
  if (!l_type.ok()) return l_type.status();

  if (*l_type == 0) {
    // 15-bit length;
    absl::StatusOr<int64_t> length = ParseInt(bits, offset, 15);
    if (!length.ok()) return length.status();

    int64_t bit_end = offset + *length;
    while (offset < bit_end) {
      absl::StatusOr<BitsExpr> sub = ParseExpr(bits, offset);
      if (!sub.ok()) return sub.status();
      ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(*sub)));
    }
    return ret;
  }
  // 11-bit subpacket count;
  absl::StatusOr<int64_t> sub_count = ParseInt(bits, offset, 11);
  if (!sub_count.ok()) return sub_count.status();
  for (int i = 0; i < *sub_count; ++i) {
    absl::StatusOr<BitsExpr> sub = ParseExpr(bits, offset);
    if (!sub.ok()) return sub.status();
    ret.sub_.push_back(absl::make_unique<BitsExpr>(std::move(*sub)));
  }
  return ret;
}

absl::StatusOr<int64_t> BitsExpr::ParseInt(const std::vector<bool>& bits,
                                           int64_t& offset, int64_t count) {
  if (offset + count > bits.size()) return Error("Read past end of bits");
  CHECK(offset < bits.size());
  int64_t v = 0;
  for (int i = 0; i < count; ++i) {
    v = v * 2 + bits[offset + i];
  }
  offset += count;
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
    case 0: {
      int64_t ret = 0;
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret += *sv;
      }
      return ret;
    }
    case 1: {
      int64_t ret = 1;
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret *= *sv;
      }
      return ret;
    }
    case 2: {
      int64_t ret = std::numeric_limits<int64_t>::max();
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret = std::min(ret, *sv);
      }
      return ret;
    }
    case 3: {
      int64_t ret = std::numeric_limits<int64_t>::min();
      for (const auto& sub : sub_) {
        absl::StatusOr<int64_t> sv = sub->Evaluate();
        if (!sv.ok()) return sv.status();
        ret = std::max(ret, *sv);
      }
      return ret;
    }
    case 4: {
      return literal_;
    }
    case 5: {
      if (sub_.size() != 2) return Error("Bad >");
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left > *right;
    }
    case 6: {
      if (sub_.size() != 2) return Error("Bad >");
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left < *right;
    }
    case 7: {
      if (sub_.size() != 2) return Error("Bad ==");
      absl::StatusOr<int64_t> left = sub_[0]->Evaluate();
      if (!left.ok()) return left.status();
      absl::StatusOr<int64_t> right = sub_[1]->Evaluate();
      if (!right.ok()) return right.status();
      return *left == *right;
    }
    default: {
      LOG(FATAL) << "Bad type: " << type_;
    }
  }
  LOG(FATAL) << "Cannot reach";
}

}  // namespace

absl::StatusOr<std::string> Day_2021_16::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<BitsExpr> expr = BitsExpr::Parse(input[0]);
  if (!expr.ok()) return expr.status();
  return IntReturn(expr->SumVersions());
}

absl::StatusOr<std::string> Day_2021_16::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<BitsExpr> expr = BitsExpr::Parse(input[0]);
  if (!expr.ok()) return expr.status();
  return IntReturn(expr->Evaluate());
}

}  // namespace advent_of_code
