#include "advent_of_code/2019/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

int CalcSumRange(std::string_view input, int begin, int end) {
  int ret = 0;
  for (int i = begin; i < end; ++i) {
    ret += input[i] - '0';
  }
  return ret;
}

class SumRangeState {
 public:
  SumRangeState(std::string_view input) : input_(input) {
    Build();
    DCHECK(Audit().ok());
  }

  // Returns CalcSumRange(input_, begin, end) in O(log(end - begin)) time.
  int SumRange(int begin, int end) const {
    return SumRangeUnaligned(begin, end);
  }

 private:
  void Build() {
    int length = input_.size() / 2;
    std::vector<int> next_sums(length);
    for (int i = 0; i < length; ++i) {
      next_sums[i] = input_[2 * i] + input_[2 * i + 1] - '0' - '0';
    }
    sums_.push_back(std::move(next_sums));
    for (length /= 2; length; length /= 2) {
      next_sums.clear();
      next_sums.resize(length);
      for (int i = 0; i < length; ++i) {
        next_sums[i] = sums_.back()[2 * i] + sums_.back()[2 * i + 1];
      }
      sums_.push_back(std::move(next_sums));
    }
  }

  absl::Status Audit() const {
    for (int shift = 0; (1 << shift) < input_.length(); ++shift) {
      int stride = (1 << shift);
      for (int begin = 0; begin < input_.length(); begin += stride) {
        if (begin + stride > input_.length()) break;
        int a_sum = SumRangeAligned(begin, shift);
        int b_sum = CalcSumRange(input_, begin, begin + stride);
        if (a_sum != b_sum) {
          return Error(begin, "+", stride, "; ", a_sum, " != ", b_sum);
        }
      }
    }
    return absl::OkStatus();
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

  std::string_view input_;
  // sums_[a][b] stores CalcSumRange(input_, b * (2 ** a), (b + 1) * (2 ** a)).
  std::vector<std::vector<int>> sums_;
};

std::string RunPhase(int phase, std::string_view input, int min_position) {
  std::string_view sub_input = input.substr(min_position);

  SumRangeState sum_range(sub_input);
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

absl::StatusOr<std::string> Day_2019_16::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Need only 1 line");

  std::string ret;
  ret = std::string(input[0]);
  for (int i = 0; i < 100; ++i) {
    ret = RunPhase(i, ret, 0);
  }
  ret = ret.substr(0, 8);

  return ret;
}

absl::StatusOr<std::string> Day_2019_16::Part2(
    absl::Span<std::string_view> input) const {
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

  return ret;
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/16,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_16()); });

}  // namespace advent_of_code
