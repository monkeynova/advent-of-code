#include "advent_of_code/2020/day23/day23.h"

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

class Cups {
 public:
  Cups() = default;

  void AddCup(int val);
  void Finalize();
  void RunMove2();

  absl::StatusOr<std::string> Label() const;
  absl::StatusOr<int64_t> Next2Product() const;

  friend std::ostream& operator<<(std::ostream& o, const Cups& c) {
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

 private:
  struct LLCup {
    int val;
    int next;
  };

  int cur_idx = 0;
  absl::flat_hash_map<int, int> cup_to_index;
  std::vector<LLCup> cups;
};

void Cups::AddCup(int val) {
  int idx = cups.size();
  cup_to_index[val] = idx;
  cups.push_back({.val = val, .next = idx + 1});
}

void Cups::Finalize() {
  cups.back().next = 0;
}

void Cups::RunMove2() {
  int cur_cup = cups[cur_idx].val;
  int dest_cup = cur_cup - 1;
  if (dest_cup < 1) dest_cup = cups.size();
  bool found = true;
  int plus_three_idx;
  while (found) {
    found = false;
    int idx = cups[cur_idx].next;
    for (int i = 0; i < 3; ++i) {
      if (cups[idx].val == dest_cup) {
        --dest_cup;
        if (dest_cup < 1) dest_cup = cups.size();
        found = true;
        break;
      }
      plus_three_idx = idx;
      idx = cups[idx].next;
    }
  }
  VLOG(2) << "  dest_cup=" << dest_cup;
  int dest_cup_idx = cup_to_index[dest_cup];
  int tmp = cups[dest_cup_idx].next;
  cups[dest_cup_idx].next = cups[cur_idx].next;
  cups[cur_idx].next = cups[plus_three_idx].next;
  cups[plus_three_idx].next = tmp;
  cur_idx = cups[cur_idx].next;
}

absl::StatusOr<std::string> Cups::Label() const {
  std::string ret;
  int ret_i = 0;
  ret.resize(cups.size() - 1);
  auto it = cup_to_index.find(1);
  if (it == cup_to_index.end()) return Error("Could not find 1");
  for (int idx = cups[it->second].next; cups[idx].val != 1;
       idx = cups[idx].next) {
    ret[ret_i] = cups[idx].val + '0';
    ++ret_i;
  }
  return ret;
}

absl::StatusOr<int64_t> Cups::Next2Product() const {
  auto it = cup_to_index.find(1);
  if (it == cup_to_index.end()) return Error("Could not find 1");
  int next_idx = cups[it->second].next;
  int64_t product = 1;
  product *= cups[next_idx].val;
  next_idx = cups[next_idx].next;
  product *= cups[next_idx].val;
  return product;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_23::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");

  Cups cups;
  for (int i = 0; i < input[0].size(); ++i) {
    cups.AddCup(input[0][i] - '0');
  }
  cups.Finalize();
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    cups.RunMove2();
  }
  VLOG(1) << cups;
  return AdventReturn(cups.Label());
}

absl::StatusOr<std::string> Day_2020_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  for (int i = 0; i < input[0].size(); ++i) {
    cups.AddCup(input[0][i] - '0');
  }
  for (int i = input[0].size(); i < 1000000; ++i) {
    cups.AddCup(i + 1);
  }
  cups.Finalize();
  for (int i = 0; i < 10000000; ++i) {
    VLOG_IF(1, i % 777777 == 0) << i;
    cups.RunMove2();
  }

  return AdventReturn(cups.Next2Product());
}

}  // namespace advent_of_code
