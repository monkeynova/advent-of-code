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

int64_t ParseInt(const std::vector<bool>& bits, int64_t& offset, int64_t count) {
  CHECK(offset < bits.size());
  int64_t v = 0;
  for (int i = 0; i < count; ++i) {
    v = v * 2 + bits[offset + i];
  }
  offset += count;
  return v;  
}

int64_t CountVersionNumbers(const std::vector<bool>& bits, int64_t& offset) {
  //VLOG(1) << offset << "/" << bits.size();
  int64_t ret = 0;
  int64_t v = ParseInt(bits, offset, 3);
  //VLOG(1) << "v:" << v;
  ret += v;
  int64_t t = ParseInt(bits, offset, 3);
  //VLOG(1) << "t:" << t;
  if (t == 4) {
    // literal. not parsed.
    //VLOG(1) << "literal";
    while (bits[offset]) {
      offset += 5;
    }
    offset += 5;
  } else {
    int64_t l_type = ParseInt(bits, offset, 1);
    //VLOG(1) << "l_type: " << l_type;
    if (l_type == 0) {
      // 15-bit length;
      int64_t l = ParseInt(bits, offset, 15);
      //VLOG(1) << "length: " << l;
      int64_t bit_end = offset + l;
      while (offset < bit_end) {
        ret += CountVersionNumbers(bits, offset);
      }
    } else {
      // 11-bit subpacket count;
      int64_t s = ParseInt(bits, offset, 11);
      //VLOG(1) << "count: " << s;
      for (int i = 0; i < s; ++i) {
        ret += CountVersionNumbers(bits, offset);
      }
    }
  }
  return ret;
}

int64_t Evaluate(const std::vector<bool>& bits, int64_t& offset) {
  //VLOG(1) << offset << "/" << bits.size();
  int64_t v = ParseInt(bits, offset, 3);
  //VLOG(1) << "v:" << v;
  int64_t type = ParseInt(bits, offset, 3);
  //VLOG(1) << "t:" << t;
  if (type == 4) {
    // literal. not parsed.
    //VLOG(1) << "literal";
    int64_t l = 0;
    while (bits[offset]) {
      ++offset;
      int64_t n = ParseInt(bits, offset, 4);
      l <<= 4;
      l += n;
    }
    ++offset;
    int64_t n = ParseInt(bits, offset, 4);
    l <<= 4;
    l += n;
    //VLOG(1) << "literal: " << l;
    return l;
  } else {
    int64_t l_type = ParseInt(bits, offset, 1);
    //VLOG(1) << "l_type: " << l_type;
    if (l_type == 0) {
      // 15-bit length;
      int64_t l = ParseInt(bits, offset, 15);
      //VLOG(1) << "length: " << l;
      int64_t bit_end = offset + l;
      int64_t ret = 0;
      bool first = true;
      while (offset < bit_end) {
        int64_t next = Evaluate(bits, offset);
        if (first) {
          ret = next;
          first = false;
        } else {
          switch (type) {
            case 0: ret = ret + next; break;
            case 1: ret = ret * next; break;
            case 2: ret = std::min(ret, next); break;
            case 3: ret = std::max(ret, next); break;
            case 4: LOG(FATAL) << "Bad type (literal)";
            case 5: ret = ret > next; break;
            case 6: ret = ret < next; break;
            case 7: ret = ret == next; break;
            default: LOG(FATAL) << "Bad type";
          }
        }
      }
      //VLOG(1) << "op: " << type << ": " << ret;
      return ret;
    } else {
      // 11-bit subpacket count;
      int64_t s = ParseInt(bits, offset, 11);
      //VLOG(1) << "count: " << s;
      int64_t ret = 0;
      bool first = true;
      for (int i = 0; i < s; ++i) {
        int64_t next = Evaluate(bits, offset);
        if (first) {
          ret = next;
          first = false;
        } else {
          switch (type) {
            case 0: ret = ret + next; break;
            case 1: ret = ret * next; break;
            case 2: ret = std::min(ret, next); break;
            case 3: ret = std::max(ret, next); break;
            case 4: LOG(FATAL) << "Bad type (literal)";
            case 5: ret = ret > next; break;
            case 6: ret = ret < next; break;
            case 7: ret = ret == next; break;
            default: LOG(FATAL) << "Bad type";
          }
        }
      }
      //VLOG(1) << "op: " << type << ": " << ret;
      return ret;
    }
  }
  LOG(FATAL) << "Cannot reach";
}

}  // namespace

absl::StatusOr<std::string> Day_2021_16::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<bool> bits;
  for (const char c : input[0]) {
    int val = -1;
    if (c >= '0' && c <= '9') val = c - '0';
    else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
    else return Error("Bad hex");
    for (int bit = 3; bit >= 0; --bit) {
      bits.push_back(val & (1 << bit));
    }
  }
  //VLOG(1) << absl::StrJoin(bits, ",");
  int64_t offset = 0;
  return IntReturn(CountVersionNumbers(bits, offset));
  

  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2021_16::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<bool> bits;
  for (const char c : input[0]) {
    int val = -1;
    if (c >= '0' && c <= '9') val = c - '0';
    else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
    else return Error("Bad hex");
    for (int bit = 3; bit >= 0; --bit) {
      bits.push_back(val & (1 << bit));
    }
  }
  //VLOG(1) << absl::StrJoin(bits, ",");
  int64_t offset = 0;
  return IntReturn(Evaluate(bits, offset));
}

}  // namespace advent_of_code
