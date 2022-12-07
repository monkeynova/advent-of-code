// https://adventofcode.com/2015/day/12
//
// --- Day 12: JSAbacusFramework.io ---
// ------------------------------------
//
// Santa's Accounting-Elves need help balancing the books after a recent
// order. Unfortunately, their accounting software uses a peculiar
// storage format. That's where you come in.
//
// They have a JSON document which contains a variety of things: arrays
// ([1,2,3]), objects ({"a":1, "b":2}), numbers, and strings. Your first job is
// to simply find all of the numbers throughout the document and add them
// together.
//
// For example:
//
// * [1,2,3] and {"a":2,"b":4} both have a sum of 6.
//
// * [[[3]]] and {"a":{"b":4},"c":-1} both have a sum of 3.
//
// * {"a":[-1,1]} and [-1,{"a":1}] both have a sum of 0.
//
// * [] and {} both have a sum of 0.
//
// You will not encounter any strings containing numbers.
//
// What is the sum of all numbers in the document?
//
// --- Part Two ---
// ----------------
//
// Uh oh - the Accounting-Elves have realized that they double-counted
// everything red.
//
// Ignore any object (and all of its children) which has any property
// with the value "red". Do this only for objects ({...}), not arrays ([...]).
//
// * [1,2,3] still has a sum of 6.
//
// * [1,{"c":"red","b":2},3] now has a sum of 4, because the middle
// object is ignored.
//
// * {"d":"red","e":[1,2,3,4],"f":5} now has a sum of 0, because the
// entire structure is ignored.
//
// * [1,"red",5] has a sum of 6, because "red" in an array has no
// effect.

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

absl::StatusOr<int> ParseAndCountNonRed(absl::string_view* json,
                                        bool* is_red = nullptr) {
  if (is_red) *is_red = false;

  if (json->empty()) return 0;
  if ((*json)[0] == '-' || ((*json)[0] <= '9' && (*json)[0] >= '0')) {
    int len = 1;
    while (len < json->size() && (*json)[len] <= '9' && (*json)[len] >= '0') {
      ++len;
    }
    absl::string_view num = json->substr(0, len);
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
      absl::StatusOr<int> next = ParseAndCountNonRed(json);
      if (!next.ok()) return next.status();
      ret += *next;
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
      absl::StatusOr<int> next = ParseAndCountNonRed(json);
      if (!next.ok()) return next.status();
      ret += *next;
      if ((*json)[0] != ':') return Error("Missing k/v break: ", *json);
      *json = json->substr(1);
      bool is_red;
      next = ParseAndCountNonRed(json, &is_red);
      if (!next.ok()) return next.status();
      ret += *next;
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
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::string_view json = input[0];
  int sum = 0;
  int next = 0;
  while (RE2::FindAndConsume(&json, "(-?\\d+)", &next)) {
    sum += next;
  }
  return IntReturn(sum);
}

absl::StatusOr<std::string> Day_2015_12::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::string_view json = input[0];
  absl::StatusOr<int> ret = ParseAndCountNonRed(&json);
  if (!ret.ok()) return ret.status();
  if (!json.empty()) return Error("Json not fully consumed: ", json);

  return IntReturn(*ret);
}

}  // namespace advent_of_code
