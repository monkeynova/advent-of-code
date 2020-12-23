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
  int pos = 0;
  absl::flat_hash_map<int, int> cup_to_pos;
  absl::flat_hash_map<int, int> pos_to_cup;
};

std::ostream& operator<<(std::ostream& o, const Cups& c) {
  for (int i = 0; c.pos_to_cup.contains(i); ++i) {
    if (i > 0) o << ",";
    if (i == c.pos) o << "(";
    o << c.pos_to_cup.find(i)->second;
    if (i == c.pos) o << ")";
  }
  return o;
}

absl::StatusOr<Cups> RunMove(Cups& cups) {
  int size = cups.pos_to_cup.size();
  int cur_cup = cups.pos_to_cup[cups.pos];
  int dest_cup = cur_cup - 1;
  if (dest_cup < 1) dest_cup = size;
  while ((size + cups.cup_to_pos[dest_cup] - (cups.pos + 1)) % size < 3) {
    dest_cup = (dest_cup - 1);
    if (dest_cup < 1) dest_cup = size;
  }
  VLOG(2) << "  dest_cup=" << dest_cup;
  Cups ret;
  ret.pos = -1;
  int out_pos = 0;
  for (int in_pos = 0; in_pos < size; ++in_pos) {
    if ((size + in_pos - (cups.pos + 1)) % size < 3) continue;
    ret.pos_to_cup[out_pos] = cups.pos_to_cup[in_pos];
    ret.cup_to_pos[cups.pos_to_cup[in_pos]] = out_pos;
    VLOG(2) << "  Putting " << cups.pos_to_cup[in_pos] << " at " << out_pos;
    ++out_pos;
    if (cups.pos_to_cup[in_pos] == dest_cup) {
      for (int i = 0; i < 3; ++i) {
        int src_pos = (size + cups.pos + 1 + i) % size;
        ret.pos_to_cup[out_pos] = cups.pos_to_cup[src_pos];
        ret.cup_to_pos[cups.pos_to_cup[src_pos]] = out_pos;
        VLOG(2) << "  Putting " << cups.pos_to_cup[src_pos] << " at " << out_pos;
        ++out_pos;
      }
    }
    if (in_pos == cups.pos) {
      // We've advanced out_pos for the next loop already.
      ret.pos = out_pos % size;
    }
  }
  if (out_pos != size)return AdventDay::Error("Bad out_pos");
  if (ret.pos_to_cup.size() != size) return AdventDay::Error("Bad pos_to_cup size");
  if (ret.cup_to_pos.size() != size) return AdventDay::Error("Bad cup_to_pos size");
  if (ret.pos == -1) return AdventDay::Error("Could not find next position");
  return ret;
}

struct Cups2 {
  struct LLCup {
    int val;
    int next;
  };

  int cur_idx;
  absl::flat_hash_map<int, int> cup_to_index;
  std::vector<LLCup> cups;
};

std::ostream& operator<<(std::ostream& o, const Cups2& c) {
  o << "1";
  for (int idx = c.cups[c.cup_to_index.find(1)->second].next;
       c.cups[idx].val != 1;
       idx = c.cups[idx].next) {
    o << ",";
    if (idx == c.cur_idx) o << "(";
    o << c.cups[idx].val;
    if (idx == c.cur_idx) o << ")";
  }
  return o;
}

void RunMove2(Cups2* cups) {
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

absl::StatusOr<std::vector<std::string>> Day23_2020::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
#if 1
  Cups2 cups;
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
  for (int idx = cups.cups[cups.cup_to_index[1]].next;
       cups.cups[idx].val != 1;
       idx = cups.cups[idx].next) {
    ret[ret_i] = cups.cups[idx].val + '0';
    ++ret_i;
  }
  return std::vector<std::string>{ret};
#else
  Cups cups;
  for (int i = 0; i < input[0].size(); ++i) {
    int cup_num = input[0][i] - '0';
    cups.pos_to_cup[i] = cup_num;
    cups.cup_to_pos[cup_num] = i;
  }
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    absl::StatusOr<Cups> next = RunMove(cups);
    if (!next.ok()) return next.status();
    cups = std::move(*next);
  }
  VLOG(1) << cups;
  std::string ret;
  int size = cups.pos_to_cup.size();
  ret.resize(size - 1);
  for (int i = 0; i < size - 1; ++i) {
    ret[i] = cups.pos_to_cup[(size + i + 1 + cups.cup_to_pos[1]) % size] + '0';
  }
  return std::vector<std::string>{ret};
#endif
}

absl::StatusOr<std::vector<std::string>> Day23_2020::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups2 cups;
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
  int product = 1;
  product *= cups.cups[next_idx].val;
  next_idx = cups.cups[next_idx].next;
  product *= cups.cups[next_idx].val;

  return IntReturn(product);
}

}  // namespace advent_of_code
