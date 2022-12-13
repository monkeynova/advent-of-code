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

  bool operator<(const Cmp& o) const {
    VLOG(2) << *this << " <?< " << o;
    if (val != -1 && o.val != -1) {
      return val < o.val;
    }
    if (val != -1) {
      Cmp list_val;
      list_val.list.push_back(absl::make_unique<Cmp>());
      list_val.list.back()->val = val;
      return list_val < o;
    }
    if (o.val != -1) {
      Cmp list_val;
      list_val.list.push_back(absl::make_unique<Cmp>());
      list_val.list.back()->val = o.val;
      return *this < list_val;
    }
    for (int i = 0; i < list.size(); ++i) {
      if (i >= o.list.size()) return false;
      if (*list[i] < *o.list[i]) return true;
      if (*o.list[i] < *list[i]) return false;
    }
    if (o.list.size() > list.size()) return true;
    return false;
  }
};

std::unique_ptr<Cmp> Parse(absl::string_view& line) {
  std::unique_ptr<Cmp> ret = absl::make_unique<Cmp>();
  if (line[0] == '[') {
    line = line.substr(1);
    while (true) {
      ret->list.push_back(Parse(line));
      char c = line[0];
      line = line.substr(1);
      if (c == ']') break;
      CHECK_EQ(c, ',');
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

}  // namespace

absl::StatusOr<std::string> Day_2022_13::Part1(
    absl::Span<absl::string_view> input) const {
  int ret = 0;
  for (int i = 0; i < input.size(); i += 3) {
    if (!input[i+2].empty()) return Error("Bad input: empty");
    auto left = Parse(input[i]);
    if (!input[i].empty()) return Error("Bad line: ", input[i]);
    auto right = Parse(input[i + 1]);
    if (!input[i + 1].empty()) return Error("Bad line: ", input[i + 1]);
    if (!(*right < *left)) {
      VLOG(2) << "Found: " << (i/3) + 1;
      ret += (i/3) + 1;
    }
  }
  return IntReturn(ret);
}

absl::StatusOr<std::string> Day_2022_13::Part2(
    absl::Span<absl::string_view> input) const {
  int ret = 0;
  std::unique_ptr<Cmp> l1 = absl::make_unique<Cmp>();
  l1->list.push_back(absl::make_unique<Cmp>());
  l1->list.back()->list.push_back(absl::make_unique<Cmp>());
  l1->list.back()->list.back()->val = 2;
  std::unique_ptr<Cmp> l2 = absl::make_unique<Cmp>();
  l2->list.push_back(absl::make_unique<Cmp>());
  l2->list.back()->list.push_back(absl::make_unique<Cmp>());
  l2->list.back()->list.back()->val = 6;

  std::vector<std::unique_ptr<Cmp>> list;
  list.push_back(std::move(l1));
  list.push_back(std::move(l2));

  for (absl::string_view line : input) {
    if (line.empty()) continue;
    list.push_back(Parse(line));
  }
  absl::c_sort(list, [](const std::unique_ptr<Cmp>& a, const std::unique_ptr<Cmp>& b) { return *a < *b; });

  std::unique_ptr<Cmp> f1 = absl::make_unique<Cmp>();
  f1->list.push_back(absl::make_unique<Cmp>());
  f1->list.back()->list.push_back(absl::make_unique<Cmp>());
  f1->list.back()->list.back()->val = 2;
  std::unique_ptr<Cmp> f2 = absl::make_unique<Cmp>();
  f2->list.push_back(absl::make_unique<Cmp>());
  f2->list.back()->list.push_back(absl::make_unique<Cmp>());
  f2->list.back()->list.back()->val = 6;

  int idx1 = -1, idx2 = -1;
  for (int i = 0; i < list.size(); ++i) {
    if (!(*list[i] < *f1) && !(*f1 < *list[i])) idx1 = i + 1;
    if (!(*list[i] < *f2) && !(*f2 < *list[i])) idx2 = i + 1;
  }
  return IntReturn(idx1 * idx2);
}

}  // namespace advent_of_code
