#include "advent_of_code/2019/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

// Problem could be solved with a sparse matrix multiply and expontentiation.
// 100 = 64 + 32 + 4. M^2^2^2^2^2^2 * M^2^2^2^2^2 * M^2^2.

int CalcSumRange(absl::string_view input, int begin, int end) {
  int ret = 0;
  for (int i = begin; i < end; ++i) {
    ret += input[i] - '0';
  }
  return ret;
}

class SumRangeState {
 public:
  SumRangeState(absl::string_view input) : input_(input) {}

  int SumRange(int begin, int end) const {
    return SumRangeUnaligned(begin, end);
  }

  void Build() {
    for (int length = input_.size() / 2; length; length /= 2) {
      std::vector<int> next_sums;
      next_sums.resize(length);
      if (length == input_.size() / 2) {
        for (int i = 0; i < length; ++i) {
          next_sums[i] = input_[2 * i] + input_[2 * i + 1] - '0' - '0';
        }
      } else {
        for (int i = 0; i < length; ++i) {
          next_sums[i] = sums_.back()[2 * i] + sums_.back()[2 * i + 1];
        }
      }
      sums_.push_back(std::move(next_sums));
      next_sums.clear();
    }

    AuditSums();
  }

 private:
  void AuditSums() const {
    for (int shift = 0; (1 << shift) < input_.length(); ++shift) {
      int stride = (1 << shift);
      for (int begin = 0; begin < input_.length(); begin += stride) {
        if (begin + stride > input_.length()) break;
        int a_sum = SumRangeAligned(begin, shift);
        int b_sum = CalcSumRange(input_, begin, begin + stride);
        if (a_sum != b_sum) {
          LOG(WARNING) << begin << "+" << stride << "; " << a_sum
                       << " != " << b_sum;
        }
      }
    }
  }

  int SumRangeUnaligned(int begin, int end) const {
    VLOG(2) << "UnAligned: " << begin << "-" << end;
    int ret = 0;
    int low_bit = 1;
    int shift = 0;
    while (begin + low_bit <= end) {
      VLOG(3) << "  " << begin << "/" << low_bit;
      if (begin & low_bit) {
        ret += SumRangeAligned(begin, shift);
        begin += low_bit;
      }
      low_bit <<= 1;
      ++shift;
    }
    while (low_bit) {
      VLOG(3) << "  " << begin << "/" << low_bit;
      if (begin + low_bit <= end) {
        ret += SumRangeAligned(begin, shift);
        begin += low_bit;
      }
      low_bit >>= 1;
      --shift;
    }
    return ret;
  }

  inline int SumRangeAligned(int begin, int sums_idx) const {
    if (sums_idx == 0) {
      return input_[begin] - '0';
    } else {
      return sums_[sums_idx - 1][begin / (1 << sums_idx)];
    }
  }

  absl::string_view input_;
  std::vector<std::vector<int>> sums_;
};

std::string RunPhase(int phase, absl::string_view input, int min_position) {
  absl::string_view sub_input = input.substr(min_position);

  SumRangeState sum_range(sub_input);
  sum_range.Build();
  std::string ret;
  ret.resize(input.size());
  for (int i = min_position; i < input.size(); ++i) {
    int stride = i + 1;
    bool negate = false;
    int64_t sum = 0;
    for (int j = i - min_position; j < sub_input.size(); j += stride * 2) {
      int range_end = std::min<int>(j + stride, sub_input.size());
      if (negate) {
        sum -= sum_range.SumRange(j, range_end);
        negate = false;
      } else {
        sum += sum_range.SumRange(j, range_end);
        negate = true;
      }
    }
    ret[i] = (abs(sum) % 10) + '0';
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day16_2019::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Need only 1 line");

  std::string ret;
  ret = std::string(input[0]);
  for (int i = 0; i < 100; ++i) {
    ret = RunPhase(i, ret, 0);
  }
  ret = ret.substr(0, 8);

  return std::vector<std::string>{ret};
}

absl::StatusOr<std::vector<std::string>> Day16_2019::Part2(
    absl::Span<absl::string_view> input) const {
  // TODO(@monkeynova): This model still takes way too long. There aught
  // to sitll be opportunities to improve the runtime.
  int offset;
  if (!absl::SimpleAtoi(input[0].substr(0, 7), &offset)) {
    return absl::InvalidArgumentError("bad atoi");
  }
  std::string ret;
  for (int i = 0; i < 10000; ++i) {
    ret.append(input[0].data(), input[0].size());
  }
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << "Phase: " << i;
    ret = RunPhase(i, ret, offset);
  }
  if (ret.size() < offset + 8)
    return absl::InvalidArgumentError("can't extract value");
  ret = ret.substr(offset, 8);

  return std::vector<std::string>{ret};
}

}  // namespace advent_of_code
