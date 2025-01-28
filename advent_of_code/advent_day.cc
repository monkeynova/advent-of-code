#include "advent_of_code/advent_day.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"

ABSL_FLAG(bool, advent_day_run_audit, true,
          "If true, runs additional tests as part of the day. Disable this "
          "flag for truer benchmarking numbers.");

ABSL_FLAG(bool, advent_day_run_compete_checks, false,
          "If true, runs lightweight checks that appropriate environment "
          "variables necessary for efficient competition are set.")
    .OnUpdate([]() {
      if (absl::GetFlag(FLAGS_advent_day_run_compete_checks)) {
        bool exit = false;
        if (getenv("USE_BAZEL_VERSION") == nullptr) {
          exit = true;
          std::cerr << "USE_BAZEL_VERSION must be set to avoid potential "
                    << "network delays or hangs with bazelisk checking for new "
                    << "versions of bazel" << std::endl;
          std::cerr << "Run: export USE_BAZEL_VERSION=`bazel --version | "
                    << "cut -f2 -d\\ `" << std::endl;
        }
        if (getenv("AOC_SESSION") == nullptr) {
          exit = true;
          std::cerr << "AOC_SESSION must be set to allow user-specific "
                    << "downloading of a days input data" << std::endl;
          std::cerr << "Log intp http://adventofcode.com/ in a browser, then "
                    << "copy the value of the session cookie into the "
                    << "following: " << std::endl;
          std::cerr << "export AOC_SESSION=<session cookie value>" << std::endl;
        }
        if (exit) {
          _exit(1);
        }
      }
    });

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
  std::unique_ptr<AdventDay> ret = it->second();
  ret->year_ = year;
  ret->day_ = day;
  return ret;
}

std::vector<int> AllAdventYears() {
  std::vector<int> years;
  absl::flat_hash_set<int> year_uniq;
  for (const auto& [year_and_day, _] : Registry()) {
    const auto [year, day] = year_and_day;
    if (year_uniq.insert(year).second) {
      years.push_back(year);
    }
  }
  absl::c_sort(years);
  return years;
}

}  // namespace advent_of_code
