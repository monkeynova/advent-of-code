#include "advent_of_code/2019/day16/day16.h"
#include "advent_of_code/file_benchmark.h"
#include "gmock/gmock.h"

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<std::vector<DirtyTestParseResult>> tests =
      FileBenchmarkTests();
  ASSERT_TRUE(tests.ok()) << tests.status().message();
  EXPECT_EQ(kMaxTestCount, tests->size());
}

BENCHMARK_TEMPLATE(BM_Day, Day16_2019)->DenseRange(0, kMaxTestCount - 1);
