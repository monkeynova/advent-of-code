// http://adventofcode.com/2022/day/13

#include "advent_of_code/2022/day13/day13.h"

#include "absl/algorithm/container.h"
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

struct Cmp {
  int val = -1;
  std::vector<std::unique_ptr<Cmp>> list;

  friend std::ostream& operator<<(std::ostream& o, const Cmp& cmp) {
    if (cmp.val != -1) return o << cmp.val;
    o << "[";
    for (int i = 0; i < cmp.list.size(); ++i) {
      if (i > 0) o << ",";
      o << *cmp.list[i];
    }
    return o << "]";
  }

  bool operator==(const Cmp& o) const {
    return cmp(o) == 0;
  }

  bool operator<(const Cmp& o) const {
    return cmp(o) < 0;
  }

  int cmp(const Cmp& o) const {
    if (val != -1 && o.val != -1) {
      return val < o.val ? -1 : val > o.val ? 1 : 0;
    }
    if (val != -1) {
      Cmp list_val;
      list_val.list.push_back(absl::make_unique<Cmp>());
      list_val.list.back()->val = val;
      return list_val.cmp(o);
    }
    if (o.val != -1) {
      Cmp list_val;
      list_val.list.push_back(absl::make_unique<Cmp>());
      list_val.list.back()->val = o.val;
      return cmp(list_val);
    }
    for (int i = 0; i < list.size(); ++i) {
      if (i >= o.list.size()) return 1;
      int sub_cmp = list[i]->cmp(*o.list[i]);
      if (sub_cmp != 0) return sub_cmp;
    }
    if (o.list.size() > list.size()) return -1;
    return 0;
  }
};

std::unique_ptr<Cmp> Parse(absl::string_view& line) {
  std::unique_ptr<Cmp> ret = absl::make_unique<Cmp>();
  if (line[0] == '[') {
    line = line.substr(1);
    while (true) {
      if (line[0] == ']') {
        line = line.substr(1);
        break;
      }
      ret->list.push_back(Parse(line));
      if (line[0] == ',') line = line.substr(1);
    }
  } else if (line[0] >= '0' && line[0] <= '9') {
    ret->val = 0;
    while (line[0] >= '0' && line[0] <= '9') {
      ret->val = ret->val * 10 + line[0] - '0';
      line = line.substr(1);
    }
  }
  return ret;
}

bool ByPointerLt(const std::unique_ptr<Cmp>& a,
                 const std::unique_ptr<Cmp>& b) {
  return *a < *b;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_13::Part1(
    absl::Span<absl::string_view> input) const {
  int ret = 0;
  for (int i = 0; i < input.size(); i += 3) {
    if (!input[i+2].empty()) return Error("Bad input: empty");
    absl::string_view consume;
    auto left = Parse(consume = input[i]);
    if (!consume.empty()) return Error("Bad line: ", input[i]);
    auto right = Parse(consume = input[i+1]);
    if (!consume.empty()) return Error("Bad line: ", input[i + 1]);
    if (!(*right < *left)) ret += (i/3) + 1;
  }
  return IntReturn(ret);
}

absl::StatusOr<std::string> Day_2022_13::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<std::unique_ptr<Cmp>> list;
  absl::string_view consume;
  list.push_back(Parse(consume = "[[2]]"));
  list.push_back(Parse(consume = "[[6]]"));

  for (absl::string_view line : input) {
    if (line.empty()) continue;
    list.push_back(Parse(line));
  }
  absl::c_sort(list, ByPointerLt);

  std::unique_ptr<Cmp> f1 = Parse(consume = "[[2]]");
  std::unique_ptr<Cmp> f2 = Parse(consume = "[[6]]");

  int idx1 = -1, idx2 = -1;
  for (int i = 0; i < list.size(); ++i) {
    if (*list[i] == *f1) idx1 = i + 1;
    if (*list[i] == *f2) idx2 = i + 1;
  }
  if (idx1 == -1) return Error("marker1 not found");
  if (idx2 == -1) return Error("marker1 not found");
  return IntReturn(idx1 * idx2);
}

}  // namespace advent_of_code
