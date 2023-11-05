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

std::list<int64_t>::iterator AddOne(std::list<int64_t>::iterator in) {
  return ++in;
}

std::list<int64_t>::iterator SubOne(std::list<int64_t>::iterator in) {
  return --in;
}

void Mix(
    std::list<int64_t>& list,
    const std::vector<std::list<int64_t>::iterator>& it_list) {
  VLOG(2) << "Mix List: " << absl::StrJoin(list, ",");
  for (std::list<int64_t>::iterator it : it_list) {
    if (*it == 0) {
      // Do nothing.
    } else if (*it > 0) {
      int64_t incs = *it % (list.size() - 1);
      for (int i = 0; i < incs; ++i) {
        std::list<int64_t>::iterator next = AddOne(it);
        if (next == list.end()) {
          list.splice(AddOne(list.begin()), list, it, AddOne(it));
        } else {
          list.splice(it, list, next, AddOne(next));
        }
      }
    } else {
      int64_t decs = (-*it) % (list.size() - 1);
      for (int i = 0; i < decs; ++i) {
        if (it == list.begin()) {
          list.splice(SubOne(list.end()), list, it, AddOne(it));
        } else {
          std::list<int64_t>::iterator prev = SubOne(it);
          list.splice(prev, list, it, AddOne(it));
        }
      }
    }
    VLOG(3) << "Step List: " << absl::StrJoin(list, ",");
  }
}

int64_t Coordinates(const std::list<int64_t>& list,
                    std::list<int64_t>::const_iterator it) {
  int64_t sum = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 1000; ++j) {
      ++it;
      if (it == list.end()) it = list.begin();
    }
    sum += *it;
  }
  return sum;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> input_ints = ParseAsInts(input);
  if (!input_ints.ok()) return input_ints.status();

  std::list<int64_t> list;
  std::vector<std::list<int64_t>::iterator> list_it;
  std::list<int64_t>::iterator zero_it = list.end();

  for (int64_t v : *input_ints) {
    list.push_back(v);
    list_it.push_back(SubOne(list.end()));
    if (v == 0) {
      if (zero_it != list.end()) return Error("Duplicate 0");
      zero_it = list_it.back();
    }
  }
  if (zero_it == list.end()) return Error("No zero found");

  Mix(list, list_it);
  return AdventReturn(Coordinates(list, zero_it));
}

absl::StatusOr<std::string> Day_2022_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> input_ints = ParseAsInts(input);
  if (!input_ints.ok()) return input_ints.status();

  std::list<int64_t> list;
  std::vector<std::list<int64_t>::iterator> list_it;
  std::list<int64_t>::iterator zero_it = list.end();

  for (int64_t v : *input_ints) {
    list.push_back(811589153 * v);
    list_it.push_back(SubOne(list.end()));
    if (v == 0) {
      if (zero_it != list.end()) return Error("Duplicate 0");
      zero_it = list_it.back();
    }
  }
  if (zero_it == list.end()) return Error("No zero found");

  for (int mix = 0; mix < 10; ++mix) {
    Mix(list, list_it);
  }
  return AdventReturn(Coordinates(list, zero_it));
}

}  // namespace advent_of_code
