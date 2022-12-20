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

struct DLL {
  explicit DLL(int64_t v) : value(v) {}
  const int64_t value;
  struct DLL* prev;
  struct DLL* next;
};

std::string DebugString(DLL* root) {
  std::string ret = absl::StrCat(root->value);
  for (DLL* add = root->next; add != root; add = add->next) {
    absl::StrAppend(&ret, ",", add->value);
  }
  return ret;
}

void Mix(std::vector<std::unique_ptr<DLL>>& list) {
  DLL* root = list[0].get();
  for (const std::unique_ptr<DLL>& ent : list) {
    if (ent->value == 0) {
      // Do nothing.
    } else if (ent->value > 0) {
      int64_t incs = ent->value % (list.size() - 1);
      DLL* cur = ent.get();
      for (int i = 0; i < incs; ++i) {
        DLL* next = cur->next;
        cur->next = next->next;
        cur->next->prev = cur;
        next->prev = cur->prev;
        next->prev->next = next;
        cur->prev = next;
        next->next = cur;
      }
    } else {
      int64_t decs = (-ent->value) % (list.size() - 1);
      DLL* cur = ent.get();
      for (int i = 0; i < decs; ++i) {
        DLL* prev = cur->prev;
        cur->prev = prev->prev;
        cur->prev->next = cur;
        prev->next = cur->next;
        prev->next->prev = prev;
        cur->next = prev;
        prev->prev = cur;
      }
    }
  }
}

int64_t Coordinates(DLL* from_val) {
  int64_t sum = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 1000; ++j) {
      from_val = from_val->next;
    }
    sum += from_val->value;
  }
  return sum;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  DLL* zero_dll = nullptr;
  std::vector<std::unique_ptr<DLL>> cycle;
  for (int64_t v : *list) {
    cycle.push_back(absl::make_unique<DLL>(v));
    if (v == 0) {
      if (zero_dll != nullptr) return Error("Duplicate 0");
      zero_dll = cycle.back().get();
    }
  }
  if (zero_dll == nullptr) return Error("No zero found");

  for (int i = 0; i < cycle.size(); ++i) {
    cycle[i]->prev = cycle[(cycle.size() + i - 1) % cycle.size()].get();
    cycle[i]->next = cycle[(i + 1) % cycle.size()].get();
  }

  Mix(cycle);
  return IntReturn(Coordinates(zero_dll));
}

absl::StatusOr<std::string> Day_2022_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(input);
  if (!list.ok()) return list.status();

  DLL* zero_dll = nullptr;
  std::vector<std::unique_ptr<DLL>> cycle;
  for (int64_t v : *list) {
    cycle.push_back(absl::make_unique<DLL>(811589153 * v));
    if (v == 0) {
      if (zero_dll != nullptr) return Error("Duplicate 0");
      zero_dll = cycle.back().get();
    }
  }
  if (zero_dll == nullptr) return Error("No zero found");

  for (int i = 0; i < cycle.size(); ++i) {
    cycle[i]->prev = cycle[(cycle.size() + i - 1) % cycle.size()].get();
    cycle[i]->next = cycle[(i + 1) % cycle.size()].get();
  }

  for (int64_t mix = 0; mix < 10; ++mix) {
    Mix(cycle);
  }
  return IntReturn(Coordinates(zero_dll));
}

}  // namespace advent_of_code
