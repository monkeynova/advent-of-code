#include "advent_of_code/2016/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

void ExpandTo(std::string& str, int size) {
  while (str.size() != size) {
    int start_size = str.size();
    str.resize(2 * str.size() + 1, '0');
    for (int i = 0; i < start_size; ++i) {
      // ^1: 0 -> 1, 1 -> 0.
      str[str.size() - 1 - i] = str[i] ^ 1;
    }
    if (str.size() > size) str.resize(size);
    VLOG(2) << str;
  }
}

void Checksum(std::string& str) {
  while (str.size() % 2 == 0) {
    for (int i = 0; i + 1 < str.size(); i += 2) {
      if (str[i] == str[i + 1]) {
        str[i / 2] = '1';
      } else {
        str[i / 2] = '0';
      }
    }
    VLOG(2) << str;
    str.resize(str.size() / 2);
  }
}

std::string ExpandThenCollapse(std::string_view input, int size) {
  std::string s = std::string(input);
  int reserve = input.size();
  while (reserve < size) reserve = 2 * reserve + 1;
  s.reserve(reserve);
  VLOG(1) << "Start: " << s;
  ExpandTo(s, size);
  VLOG(1) << "Expanded: " << s;
  Checksum(s);
  VLOG(1) << "Checksum: " << s;
  return s;
}

class DragonCurve {
 public:
  explicit DragonCurve(std::string_view str) : str_(str) {}

  char NextChar() {
    if (mod_ == str_.size()) {
      ++pass_;
      int pass_stripped = pass_;
      while ((pass_stripped & 1) == 0) pass_stripped >>= 1;
      bool negate = (pass_stripped & 0b11) == 0b11;
      mod_ = 0;
      return negate ? '1' : '0';
    }
    char ret = (pass_ % 2 == 0) ? str_[mod_] : str_[str_.size() - 1 - mod_] ^ 1;
    ++mod_;
    return ret;
  }

 private:
  int pass_ = 0;
  int mod_ = 0;
  std::string_view str_;
};

// abc0xyz 0 abc1xyz  0  abc0xyz 1 abc1xyz 
// 001 0 011  0  001 1 011   0   001 0 011  1  001 1 011
// 000 0 000  0  000 0 000   1   111 1 111  1  111 1 111
// 000 0 000  1  111 1 111   0   000 0 000  1  111 1 111
// 000 1 111  0  000 1 111   0   000 1 111  0  000 1 111
// 011 0 011  0  011 0 011   0   011 0 011  0  011 0 011
// 101 0 101  0  101 0 101   0   101 0 101  0  101 0 101
std::string ExpandToStream(std::string_view str, int size) {
  std::string ret(size, '?');
  DragonCurve dragon_curve(str);
  for (int idx = 0; idx < size; ++idx) {
    ret[idx] = dragon_curve.NextChar();
  }
  return ret;
}

class CheckSum {
 public:
  CheckSum(int size) {
    for (int even_size = size; even_size % 2 == 0; even_size /= 2) {
      ++stack_size_;
    }    
    CHECK_LT(stack_size_, stack_.size());
  }

  std::string_view check_sum() const {
    CHECK_EQ(stack_idx_, -1);
    return check_sum_;
  }

  void AddChar(char c) {
    Entry top = {.val = c, .level = 0};
    while (stack_idx_ >= 0 && stack_[stack_idx_].level == top.level) {
      ++top.level;
      top.val ^= '1' ^ stack_[stack_idx_].val;
      --stack_idx_;
    }
    if (top.level == stack_size_) {
      check_sum_.append(1, top.val);
    } else {
      stack_[++stack_idx_] = top;
    }
  }

 private:
  struct Entry {
    char val;
    char level;
  };

  int stack_size_ = 0;
  std::array<Entry, 32> stack_;
  int stack_idx_ = -1;
  std::string check_sum_;
};

std::string ChecksumStream(std::string_view str) {
  CheckSum sum(str.size());
  for (char c : str) {
    sum.AddChar(c);
  }
  return std::string(sum.check_sum());
}

std::string StreamingIshChecksum(std::string_view input, int size) {
  std::string s = std::string(input);
  int reserve = input.size();
  while (reserve < size) reserve = 2 * reserve + 1;
  s.reserve(reserve);
  VLOG(1) << "Start: " << s;
  std::string expand_stream = ExpandToStream(input, size);
  s = ChecksumStream(expand_stream);
  VLOG(1) << "ChecksumStream: " << s;
  return s;
}

std::string StreamingChecksum(std::string_view input, int size) {
  DragonCurve dragon_curve(input);
  CheckSum sum(size);
  for (int idx = 0; idx < size; ++idx) {
    sum.AddChar(dragon_curve.NextChar());
  }
  return std::string(sum.check_sum());
}

}  // namespace

absl::StatusOr<std::string> Day_2016_16::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  ASSIGN_OR_RETURN(int val, IntParam());
  // ExpandThenCollapse is actually more performance so we use that for
  // benchmarking etc. (2023-11-23: About 140ms vs 160ms for Part2).
  std::string ret = ExpandThenCollapse(input[0], val);
  if (run_audit()) {
    std::string streaming_ret = StreamingChecksum(input[0], val);
    if (ret != streaming_ret) {
      return Error("Streaming Checksum failed: ", ret, " != ", streaming_ret);
    }
    std::string streaming_ish_ret = StreamingIshChecksum(input[0], val);
    if (ret != streaming_ish_ret) {
      return Error("Streaming Checksum failed: ", ret, " != ", streaming_ish_ret);
    }
  }
  return AdventReturn(ret);
}

absl::StatusOr<std::string> Day_2016_16::Part2(
    absl::Span<std::string_view> input) const {
  return Part1(input);
}

}  // namespace advent_of_code
