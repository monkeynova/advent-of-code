// http://adventofcode.com/2022/day/20

#include "advent_of_code/2022/day20/day20.h"

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

struct NumAndOrder {
  int64_t value;
  int64_t order;
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const NumAndOrder& no) {
    absl::Format(&sink, "%d@%d", no.value, no.order);
  }
  friend std::ostream& operator<<(std::ostream& o, const NumAndOrder& no) {
    return o << no.value << "@" << no.order;
  }
};

absl::Status Mix(std::list<NumAndOrder>& moved) {
  for (int64_t order = 0; order < moved.size(); ++order) {
    VLOG(2) << "moving: #" << order;
    VLOG(2) << absl::StrJoin(moved, ",");
    bool found = false;
    for (auto it = moved.begin(); it != moved.end(); ++it) {
      if (it->order != order) continue;
      found = true;
      int64_t v = it->value;
      if (v == 0) break;
      it = moved.erase(it);
      if (it == moved.end()) it = moved.begin();
      if (v > 0) {
        int64_t incs = v % moved.size();
        for (int64_t i = 0; i < incs; ++i) {
          ++it;
          if (it == moved.end()) it = moved.begin();
        }
      } else {
        int64_t decs = (-v) % moved.size();
        for (int64_t i = 0; i < decs; ++i) {
          if (it == moved.begin()) it = moved.end();
          --it;
        }
      }
      moved.insert(it, {v, order});
      break;
    }
    if (!found) return Error("Could not move #", order);
  }
  return absl::OkStatus();
}

absl::StatusOr<int64_t> Coordinates(std::list<NumAndOrder>& moved) {
  auto zero_it = moved.end();
  for (auto it = moved.begin(); it != moved.end(); ++it) {
    if (it->value == 0) {
      if (zero_it != moved.end()) return Error("Not unqiue");
      zero_it = it;
    }
  }
  if (zero_it == moved.end()) return Error("No 0 found");
  int64_t sum = 0;
  auto sum_it = zero_it;
  for (int64_t i = 0; i < 3; ++i) {
    for (int64_t j = 0; j < 1000; ++j) {
      ++sum_it;
      if (sum_it == moved.end()) sum_it = moved.begin();
    }
    sum += sum_it->value;
  }
  return sum;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  std::list<NumAndOrder> moved;
  for (int64_t i = 0; i < list->size(); ++i) {
    moved.push_back(NumAndOrder{.value = (*list)[i], .order = i});
  }

  absl::Status st = Mix(moved);
  if (!st.ok()) return st;

  return IntReturn(Coordinates(moved));
}

absl::StatusOr<std::string> Day_2022_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  std::list<NumAndOrder> moved;
  for (int64_t i = 0; i < list->size(); ++i) {
    moved.push_back(NumAndOrder{.value = (*list)[i] * 811589153, .order = i});
  }

  for (int64_t mix = 0; mix < 10; ++mix) {
    VLOG(2) << "mix: #" << mix;
    VLOG(2) << absl::StrJoin(moved, ",");
    absl::Status st = Mix(moved);
    if (!st.ok()) return st;
  }

  return IntReturn(Coordinates(moved));
}

}  // namespace advent_of_code
