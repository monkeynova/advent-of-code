#include "advent_of_code/advent_day.h"

#include "absl/container/flat_hash_map.h"

ABSL_FLAG(bool, advent_day_run_audit, true,
          "If true, runs additional tests as part of the day. Disable this "
          "flag for truer benchmarking numbers.");

namespace advent_of_code {

namespace {

using FactoryType = std::function<std::unique_ptr<AdventDay>()>;

absl::flat_hash_map<std::pair<int, int>, FactoryType>& Registry() {
  static absl::flat_hash_map<std::pair<int, int>, FactoryType> registry;
  return registry;
}

}  // namespace

absl::StatusOr<int64_t> AdventDay::IntParam() const {
  int64_t param_val;
  if (!absl::SimpleAtoi(param(), &param_val))
    return Error("Not an int: ", param());
  return param_val;
}

absl::StatusOr<int64_t> AdventDay::IntParam1() const {
  auto [p1, p2] = PairSplit(param(), ",");
  int64_t param;
  if (!absl::SimpleAtoi(p1, &param)) {
    return Error("Bad int param: ", p1);
  }
  return param;
}

absl::StatusOr<int64_t> AdventDay::IntParam2() const {
  auto [p1, p2] = PairSplit(param(), ",");
  int64_t param;
  if (!absl::SimpleAtoi(p2.empty() ? p1 : p2, &param)) {
    return Error("Bad int param: ", p2.empty() ? p1 : p2);
  }
  return param;
}

AdventRegisterEntry RegisterAdventDay(int year, int day, FactoryType factory) {
  CHECK(Registry().insert({{year, day}, factory}).second);
  return AdventRegisterEntry{};
}

std::unique_ptr<AdventDay> CreateAdventDay(int year, int day) {
  auto it = Registry().find({year, day});
  if (it == Registry().end()) return nullptr;
  return it->second();
}

}  // namespace advent_of_code
