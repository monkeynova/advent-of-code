#include "advent_of_code/2020/day23/day23.h"

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

struct Cups {
  struct LLCup {
    int val;
    int next;
  };

  int cur_idx;
  absl::flat_hash_map<int, int> cup_to_index;
  std::vector<LLCup> cups;
};

std::ostream& operator<<(std::ostream& o, const Cups& c) {
  o << "1";
  for (int idx = c.cups[c.cup_to_index.find(1)->second].next;
       c.cups[idx].val != 1; idx = c.cups[idx].next) {
    o << ",";
    if (idx == c.cur_idx) o << "(";
    o << c.cups[idx].val;
    if (idx == c.cur_idx) o << ")";
  }
  return o;
}

void RunMove2(Cups* cups) {
  int cur_cup = cups->cups[cups->cur_idx].val;
  int dest_cup = cur_cup - 1;
  if (dest_cup < 1) dest_cup = cups->cups.size();
  bool found = true;
  int plus_three_idx;
  while (found) {
    found = false;
    int idx = cups->cups[cups->cur_idx].next;
    for (int i = 0; i < 3; ++i) {
      if (cups->cups[idx].val == dest_cup) {
        --dest_cup;
        if (dest_cup < 1) dest_cup = cups->cups.size();
        found = true;
        break;
      }
      plus_three_idx = idx;
      idx = cups->cups[idx].next;
    }
  }
  VLOG(2) << "  dest_cup=" << dest_cup;
  int dest_cup_idx = cups->cup_to_index[dest_cup];
  int tmp = cups->cups[dest_cup_idx].next;
  cups->cups[dest_cup_idx].next = cups->cups[cups->cur_idx].next;
  cups->cups[cups->cur_idx].next = cups->cups[plus_three_idx].next;
  cups->cups[plus_three_idx].next = tmp;
  cups->cur_idx = cups->cups[cups->cur_idx].next;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_23::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  cups.cur_idx = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    int cup_num = input[0][i] - '0';
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  cups.cups.back().next = 0;
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    RunMove2(&cups);
  }
  VLOG(1) << cups;
  std::string ret;
  int ret_i = 0;
  ret.resize(cups.cups.size() - 1);
  for (int idx = cups.cups[cups.cup_to_index[1]].next; cups.cups[idx].val != 1;
       idx = cups.cups[idx].next) {
    ret[ret_i] = cups.cups[idx].val + '0';
    ++ret_i;
  }
  return ret;
}

absl::StatusOr<std::string> Day_2020_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  cups.cur_idx = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    int cup_num = input[0][i] - '0';
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  for (int i = input[0].size(); i < 1000000; ++i) {
    int cup_num = i + 1;
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  cups.cups.back().next = 0;
  for (int i = 0; i < 10000000; ++i) {
    VLOG_IF(1, i % 777777 == 0) << i;
    RunMove2(&cups);
  }
  int next_idx = cups.cups[cups.cup_to_index[1]].next;
  int64_t product = 1;
  product *= cups.cups[next_idx].val;
  next_idx = cups.cups[next_idx].next;
  product *= cups.cups[next_idx].val;

  return IntReturn(product);
}

}  // namespace advent_of_code
