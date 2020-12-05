#include "advent_of_code/2019/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

// Problem could be solved with a sparse matrix multiply and expontentiation.
// 100 = 64 + 32 + 4. M^2^2^2^2^2^2 * M^2^2^2^2^2 * M^2^2.

int CalcSumRange(absl::string_view input, int begin, int end) {
  int ret = 0;
  for (int i = begin; i < end; ++i) {
    ret += input[i] - '0';
  }
  return ret;
}

struct SumRangeState {
  absl::string_view input;
  std::vector<std::vector<int>> sums;
};

void BuildAlignedSums(SumRangeState* state) {
  for (int length = state->input.size() / 2; length; length /= 2) {
    std::vector<int> next_sums;
    next_sums.resize(length);
    if (length == state->input.size() / 2) {
      for (int i = 0; i < length; ++i) {
        next_sums[i] =
            state->input[2 * i] + state->input[2 * i + 1] - '0' - '0';
      }
    } else {
      for (int i = 0; i < length; ++i) {
        next_sums[i] =
            state->sums.back()[2 * i] + state->sums.back()[2 * i + 1];
      }
    }
    state->sums.push_back(std::move(next_sums));
    next_sums.clear();
  }
}

inline int SumRangeAligned(SumRangeState* state, int begin, int sums_idx) {
  if (sums_idx == 0) {
    return state->input[begin] - '0';
  } else {
    return state->sums[sums_idx - 1][begin / (1 << sums_idx)];
  }
}

int SumRangeUnaligned(SumRangeState* state, int begin, int end) {
  VLOG(2) << "UnAligned: " << begin << "-" << end;
  int ret = 0;
  int low_bit = 1;
  int shift = 0;
  while (begin + low_bit <= end) {
    VLOG(3) << "  " << begin << "/" << low_bit;
    if (begin & low_bit) {
      ret += SumRangeAligned(state, begin, shift);
      begin += low_bit;
    }
    low_bit <<= 1;
    ++shift;
  }
  while (low_bit) {
    VLOG(3) << "  " << begin << "/" << low_bit;
    if (begin + low_bit <= end) {
      ret += SumRangeAligned(state, begin, shift);
      begin += low_bit;
    }
    low_bit >>= 1;
    --shift;
  }
  return ret;
}

int SumRange(SumRangeState* state, int begin, int end) {
  return SumRangeUnaligned(state, begin, end);
}

void AuditSums(SumRangeState* state) {
  for (int shift = 0; (1 << shift) < state->input.length(); ++shift) {
    int stride = (1 << shift);
    for (int begin = 0; begin < state->input.length(); begin += stride) {
      if (begin + stride > state->input.length()) break;
      int a_sum = SumRangeAligned(state, begin, shift);
      int b_sum = CalcSumRange(state->input, begin, begin + stride);
      if (a_sum != b_sum) {
        LOG(WARNING) << begin << "+" << stride << "; " << a_sum
                     << " != " << b_sum;
      }
    }
  }
}

std::string RunPhase(int phase, absl::string_view input) {
  SumRangeState sum_range{input};
  BuildAlignedSums(&sum_range);
  AuditSums(&sum_range);
  std::string ret;
  ret.resize(input.size());
  for (int i = 0; i < input.size(); ++i) {
    int stride = i + 1;
    bool negate = false;
    int64_t sum = 0;
    for (int j = i; j < input.size(); j += stride * 2) {
      int delta =
          SumRange(&sum_range, j, std::min<int>(j + stride, input.size()));
      if (negate) {
        sum -= delta;
      } else {
        sum += delta;
      }
      negate = !negate;
    }
    ret[i] = (abs(sum) % 10) + '0';
  }
  return ret;
}

absl::StatusOr<std::vector<std::string>> Day16_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Need only 1 line");

  std::string ret;
  ret = std::string(input[0]);
  for (int i = 0; i < 100; ++i) {
    ret = RunPhase(i, ret);
  }
  ret = ret.substr(0, 8);

  return std::vector<std::string>{ret};
}

absl::StatusOr<std::vector<std::string>> Day16_2019::Part2(
    const std::vector<absl::string_view>& input) const {
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
    ret = RunPhase(i, ret);
  }
  if (ret.size() < offset + 8)
    return absl::InvalidArgumentError("can't extract value");
  ret = ret.substr(offset, 8);

  return std::vector<std::string>{ret};
}
