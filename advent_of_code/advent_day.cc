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
