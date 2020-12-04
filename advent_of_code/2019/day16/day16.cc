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

int64_t FilterWeight(int64_t output_position, int64_t input_position) {
  constexpr int64_t kBasePattern[] = {0, 1, 0, -1};
  constexpr int kPatternSize = sizeof(kBasePattern) / sizeof(kBasePattern[0]);
  int index = (kPatternSize + (input_position + 1) / (output_position + 1)) % kPatternSize;
  VLOG(3) << output_position << ", " << input_position << " => " << index;
  return kBasePattern[index];
}

struct PhaseValueState {
  absl::string_view base_input;
  int64_t repeat = 1;
  std::vector<std::string> memo;

  uint64_t length() { return base_input.size() * repeat; }
};

char PhaseValue(PhaseValueState* state, int phase, int output_position) {
  if (phase == 0) {
    return state->base_input[output_position % state->base_input.size()];
  }
  if (state->memo.empty()) {
    state->memo.resize(phase);
  }
  if (state->memo[phase-1].empty()) {
    VLOG(2) << "Allocating: " << phase;
    state->memo[phase-1].resize(state->length());
  }
  if (state->memo[phase-1][output_position] != '\0') {
    return state->memo[phase-1][output_position];
  }
  int sum = 0;
  for (int64_t input_pos = 0; input_pos < state->length() * state->repeat; ++input_pos) {
    // VLOG(3) << phase << ", " << input_pos;
    int64_t weight = FilterWeight(output_position, input_pos);
    if (weight == 0) {
      if (input_pos == 0) {
        input_pos = output_position;
      } else {
        input_pos += output_position + 1;
      }
      if (input_pos >= state->length() * state->repeat) break;
      weight = FilterWeight(output_position, input_pos);
      CHECK_NE(weight, 0);
    }
    if (weight != 0) {
      sum += weight * (PhaseValue(state, phase - 1, input_pos) - '0');
    }
  }
  state->memo[phase-1][output_position] = '0' + (abs(sum) % 10);
  VLOG(2) << phase << ", " << output_position << " => " << state->memo[phase-1][output_position];
  return state->memo[phase-1][output_position];
}

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
        next_sums[i] = state->input[2 * i] + state->input[2 * i + 1] - '0' - '0';
      }
    } else {
      for (int i = 0; i < length; ++i) {
        next_sums[i] = state->sums.back()[2 * i] + state->sums.back()[2 * i + 1];
      }
    }
    state->sums.push_back(std::move(next_sums));
    next_sums.clear();
  }
}

int SumRangeAligned(SumRangeState* state, int begin, int end) {
  int sums_idx = -1;
  int sums_sums_idx = begin;
  for (int delta = end - begin; delta / 2; delta /= 2) {
    ++sums_idx;
    sums_sums_idx /= 2;
  }
  VLOG(2) << "  Aligned: " << begin << "-" << end << " => " << sums_idx << "/" << sums_sums_idx;
  if (sums_idx == -1) {
    return state->input[begin] - '0';
  } else {
    return state->sums[sums_idx][sums_sums_idx];
  }
}

int SumRange(SumRangeState* state, int begin, int end) {
  VLOG(2) << "UnAligned: " << begin << "-" << end;
  int ret = 0;
  int low_bit = 1;
  while (begin + low_bit <= end) {
    VLOG(3) << "  " << begin << "/" << low_bit;
    if (begin & low_bit) {
      ret += SumRangeAligned(state, begin, begin + low_bit);
      begin += low_bit;
    }
    low_bit <<= 1;
  }
  while (low_bit) {
    VLOG(3) << "  " << begin << "/" << low_bit;
    if (begin + low_bit <= end) {
      ret += SumRangeAligned(state, begin, begin + low_bit);
      begin += low_bit;
    }
    low_bit >>= 1;
  }
  return ret;
}

void AuditSums(SumRangeState* state) {
  for (int stride = 1; stride < state->input.length(); stride *= 2) {
    for (int begin = 0; begin < state->input.length(); begin += stride) {
      if (begin + stride > state->input.length()) break;
      int end = begin + stride;
      int a_sum = SumRangeAligned(state, begin, end);
      int b_sum = CalcSumRange(state->input, begin, end);
      if (a_sum != b_sum) {
        LOG(WARNING) << begin << "-" << end << "; " << a_sum << " != " << b_sum;
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
      int delta = SumRange(&sum_range, j, std::min<int>(j + stride, input.size()));
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
  if (false) {
    ret.resize(8);
    PhaseValueState state{.base_input = input[0]};
    for (int i = 0; i < 8; ++i) {
      ret[i] = PhaseValue(&state, 100, i);
    }
  } else {
    ret = std::string(input[0]);
    for (int i = 0; i < 100; ++i) {
      ret = RunPhase(i, ret);
    }
    ret = ret.substr(0, 8);
  }

  return std::vector<std::string>{ret};
}

absl::StatusOr<std::vector<std::string>> Day16_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  int offset;
  if (!absl::SimpleAtoi(input[0].substr(0, 7), &offset)) {
    return absl::InvalidArgumentError("bad atoi");
  }
  std::string ret;
  if (true) {
    for (int i = 0; i < 10000; ++i) {
      ret.append(input[0].data(), input[0].size());
    }
    for (int i = 0; i < 100; ++i) {
      VLOG(1) << "Phase: " << i;
      ret = RunPhase(i, ret);
    }
    if (ret.size() < offset + 8) return absl::InvalidArgumentError("can't extract value");
    ret = ret.substr(offset, 8);
  } else {
    ret.resize(8);
    PhaseValueState state{.base_input = input[0], .repeat = 10000};
    for (int i = 0; i < 8; ++i) {
      ret[i] = PhaseValue(&state, 100, offset + i);
    }
  }

  return std::vector<std::string>{ret};
}
