#include "advent_of_code/2016/day21/day21.h"

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

absl::StatusOr<std::string> ApplyRule(absl::string_view rule,
                                      absl::string_view str) {
  int x;
  int y;
  char a;
  char b;
  if (RE2::FullMatch(rule, "swap position (\\d+) with position (\\d+)", &x,
                     &y)) {
    std::string ret = std::string(str);
    std::swap(ret[x], ret[y]);
    return ret;
  }
  if (RE2::FullMatch(rule, "swap letter ([a-z]) with letter ([a-z])", &a, &b)) {
    std::string ret = std::string(str);
    bool found = false;
    for (int x = 0; x < ret.size(); ++x) {
      if (ret[x] == a) {
        bool sub_found = false;
        for (int y = 0; y < ret.size(); ++y) {
          if (ret[y] == b) {
            std::swap(ret[x], ret[y]);
            sub_found = true;
            break;
          }
        }
        if (!sub_found) return Error("Could not find b");
        found = true;
        break;
      }
    }
    if (!found) return Error("Could not find a");
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate left (\\d+) steps?", &x)) {
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(i + x) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate right (\\d+) steps?", &x)) {
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(ret.size() + i - x) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate based on position of letter (.)", &a)) {
    x = -1;
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == a) {
        x = i;
        break;
      }
    }
    if (x == -1) return Error("Not found for rotation");
    int right_rotate = (x + ((x >= 4) ? 2 : 1)) % str.size();
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(ret.size() + i - right_rotate) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "reverse positions (\\d+) through (\\d+)", &x, &y)) {
    std::string ret = std::string(str);
    std::reverse(ret.begin() + x, ret.begin() + y + 1);
    return ret;
  }
  if (RE2::FullMatch(rule, "move position (\\d+) to position (\\d+)", &x, &y)) {
    std::string ret = std::string(str);
    if (x > y) {
      ret[y] = str[x];
      for (int i = y + 1; i <= x; ++i) {
        ret[i] = str[i - 1];
      }
    } else {
      for (int i = x; i < y; ++i) {
        ret[i] = str[i + 1];
      }
      ret[y] = str[x];
    }
    return ret;
  }

  return Error("Bad rule: ", rule);
}

absl::StatusOr<std::string> ApplyRuleReverse(absl::string_view rule,
                                             absl::string_view str) {
  int x;
  int y;
  char a;
  char b;
  if (RE2::FullMatch(rule, "swap position (\\d+) with position (\\d+)", &x,
                     &y)) {
    std::string ret = std::string(str);
    std::swap(ret[x], ret[y]);
    return ret;
  }
  if (RE2::FullMatch(rule, "swap letter ([a-z]) with letter ([a-z])", &a, &b)) {
    std::string ret = std::string(str);
    bool found = false;
    for (int x = 0; x < ret.size(); ++x) {
      if (ret[x] == a) {
        bool sub_found = false;
        for (int y = 0; y < ret.size(); ++y) {
          if (ret[y] == b) {
            std::swap(ret[x], ret[y]);
            sub_found = true;
            break;
          }
        }
        if (!sub_found) return Error("Could not find b");
        found = true;
        break;
      }
    }
    if (!found) return Error("Could not find a");
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate left (\\d+) steps?", &x)) {
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(ret.size() + i - x) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate right (\\d+) steps?", &x)) {
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(i + x) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "rotate based on position of letter (.)", &a)) {
    x = -1;
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == a) {
        x = i;
        break;
      }
    }
    if (x == -1) return Error("Not found for rotation");
    int left_rotate = -1;
    for (int src_pos = 0; src_pos < str.size(); ++src_pos) {
      int right_rotate = (src_pos + ((src_pos >= 4) ? 2 : 1)) % str.size();
      if ((src_pos + right_rotate) % str.size() == x) {
        // if (left_rotate != -1) return Error("Inverse rotation not
        // unique");
        left_rotate = right_rotate;
      }
    }
    if (left_rotate == -1) return Error("Not found for inverse rotation");
    std::string ret = std::string(str);
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = str[(i + left_rotate) % ret.size()];
    }
    return ret;
  }
  if (RE2::FullMatch(rule, "reverse positions (\\d+) through (\\d+)", &x, &y)) {
    std::string ret = std::string(str);
    std::reverse(ret.begin() + x, ret.begin() + y + 1);
    return ret;
  }
  if (RE2::FullMatch(rule, "move position (\\d+) to position (\\d+)", &x, &y)) {
    std::swap(x, y);
    std::string ret = std::string(str);
    if (x > y) {
      ret[y] = str[x];
      for (int i = y + 1; i <= x; ++i) {
        ret[i] = str[i - 1];
      }
    } else {
      for (int i = x; i < y; ++i) {
        ret[i] = str[i + 1];
      }
      ret[y] = str[x];
    }
    return ret;
  }

  return Error("Bad rule: ", rule);
}

absl::Status RunTest() {
  std::string pw = "abcde";
  std::vector<std::string> instructions = {
      "swap position 4 with position 0",
      "swap letter d with letter b",
      "reverse positions 0 through 4",
      "rotate left 1 step",
      "move position 1 to position 4",
      "move position 3 to position 0",
      "rotate based on position of letter b",
      "rotate based on position of letter d",
  };
  VLOG(1) << "Test: " << pw;
  for (absl::string_view in : instructions) {
    VLOG(1) << "Test: " << in;
    absl::StatusOr<std::string> next = ApplyRule(in, pw);
    if (!next.ok()) return next.status();
    pw = std::move(*next);
    VLOG(1) << "Test: " << pw;
  }
  if (pw != "decab") {
    return Error("wrong result: ", pw);
  }
  return absl::OkStatus();
}

absl::Status RunReverseTest() {
  std::string pw = "decab";
  std::vector<std::string> instructions = {
      "swap position 4 with position 0",
      "swap letter d with letter b",
      "reverse positions 0 through 4",
      "rotate left 1 step",
      "move position 1 to position 4",
      "move position 3 to position 0",
      "rotate based on position of letter b",
      "rotate based on position of letter d",
  };
  std::vector<absl::string_view> reverse(instructions.begin(),
                                         instructions.end());
  std::reverse(reverse.begin(), reverse.end());
  VLOG(1) << "Test: " << pw;
  for (absl::string_view in : reverse) {
    VLOG(1) << "Test: " << in;
    absl::StatusOr<std::string> next = ApplyRuleReverse(in, pw);
    if (!next.ok()) return next.status();
    pw = std::move(*next);
    VLOG(1) << "Test: " << pw;
  }
  if (pw != "abcde") {
    return Error("wrong result: ", pw);
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2016_21::Part1(
    absl::Span<absl::string_view> input) const {
  if (absl::Status st = RunTest(); !st.ok()) return st;

  std::string pw = "abcdefgh";
  VLOG(2) << pw;
  for (absl::string_view in : input) {
    VLOG(2) << in;
    absl::StatusOr<std::string> next = ApplyRule(in, pw);
    if (!next.ok()) return next.status();
    pw = std::move(*next);
    VLOG(2) << pw;
  }
  return pw;
}

absl::StatusOr<std::string> Day_2016_21::Part2(
    absl::Span<absl::string_view> input) const {
  if (absl::Status st = RunReverseTest(); !st.ok()) return st;

  std::string pw = "fbgdceah";
  VLOG(2) << pw;
  std::vector<absl::string_view> reverse(input.begin(), input.end());
  std::reverse(reverse.begin(), reverse.end());
  for (absl::string_view in : reverse) {
    VLOG(2) << in;
    absl::StatusOr<std::string> next = ApplyRuleReverse(in, pw);
    if (!next.ok()) return next.status();
    pw = std::move(*next);
    VLOG(2) << pw;
  }
  return pw;
}

}  // namespace advent_of_code
