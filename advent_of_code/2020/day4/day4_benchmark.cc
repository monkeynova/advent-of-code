#include "advent_of_code/2020/day4/day4.h"
#include "advent_of_code/file_benchmark.h"
#include "gmock/gmock.h"

constexpr int kMaxTestCount = 3;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<std::vector<DirtyTestParseResult>> tests = FileBenchmarkTests();
  ASSERT_TRUE(tests.ok()) << tests.status().message();
  EXPECT_EQ(kMaxTestCount, tests->size());
}

BENCHMARK_TEMPLATE(BM_Day, Day4_2020)->DenseRange(0, kMaxTestCount - 1);
