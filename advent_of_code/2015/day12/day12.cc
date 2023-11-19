#include "advent_of_code/2015/day12/day12.h"

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

absl::StatusOr<int> ParseAndCountNonRed(std::string_view* json,
                                        bool* is_red = nullptr) {
  if (is_red) *is_red = false;

  if (json->empty()) return 0;
  if ((*json)[0] == '-' || ((*json)[0] <= '9' && (*json)[0] >= '0')) {
    int len = 1;
    while (len < json->size() && (*json)[len] <= '9' && (*json)[len] >= '0') {
      ++len;
    }
    std::string_view num = json->substr(0, len);
    int val;
    if (!absl::SimpleAtoi(num, &val)) return Error("Not numeric: ", *json);
    *json = json->substr(len);
    return val;
  }
  if ((*json)[0] == '\"') {
    int len = 1;
    while (len < json->size() && (*json)[len] != '"') {
      ++len;
    }
    ++len;
    if (is_red) *is_red = json->substr(1, len - 2) == "red";
    *json = json->substr(len);
    return 0;
  }
  if ((*json)[0] == '[') {
    int ret = 0;
    *json = json->substr(1);
    while (!json->empty() && (*json)[0] != ']') {
      ASSIGN_OR_RETURN(int next, ParseAndCountNonRed(json));
      ret += next;
      if ((*json)[0] == ',') {
        *json = json->substr(1);
      } else if ((*json)[0] != ']') {
        return Error("Bad array: ", *json);
      }
    }
    if (json->empty()) return Error("Truncated array");
    *json = json->substr(1);
    return ret;
  }
  if ((*json)[0] == '{') {
    bool has_red = false;
    int ret = 0;
    *json = json->substr(1);
    while (!json->empty() && (*json)[0] != '}') {
      ASSIGN_OR_RETURN(int next, ParseAndCountNonRed(json));
      ret += next;
      if ((*json)[0] != ':') return Error("Missing k/v break: ", *json);
      *json = json->substr(1);
      bool is_red;
      ASSIGN_OR_RETURN(next, ParseAndCountNonRed(json, &is_red));
      ret += next;
      if (is_red) has_red = true;
      if ((*json)[0] == ',') {
        *json = json->substr(1);
      } else if ((*json)[0] != '}') {
        return Error("Bad map: ", *json);
      }
    }
    if (json->empty()) return Error("Truncated map");
    *json = json->substr(1);
    return has_red ? 0 : ret;
  }
  return Error("Cannot handle: ", *json);
}

}  // namespace

absl::StatusOr<std::string> Day_2015_12::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string_view json = input[0];
  int sum = 0;
  int next = 0;
  while (RE2::FindAndConsume(&json, "(-?\\d+)", &next)) {
    sum += next;
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2015_12::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string_view json = input[0];
  ASSIGN_OR_RETURN(int ret, ParseAndCountNonRed(&json));
  if (!json.empty()) return Error("Json not fully consumed: ", json);

  return AdventReturn(ret);
}

}  // namespace advent_of_code
