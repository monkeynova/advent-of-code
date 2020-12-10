#include "advent_of_code/file_benchmark.h"
#include "advent_of_code/2019/day24/day24.h"
#include "gmock/gmock.h"

constexpr int kMaxTestCount = 3;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<int> test_count = FileBenchmarkTestCount();
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day24_2019)->DenseRange(0, kMaxTestCount - 1);
