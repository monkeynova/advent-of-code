#include "advent_of_code/file_benchmark.h"
#include "advent_of_code/new_day_path/new_day.h"
#include "gmock/gmock.h"

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<std::vector<DirtyTestParseResult>> tests =
      FileBenchmarkTests();
  ASSERT_TRUE(tests.ok()) << tests.status().message();
  EXPECT_EQ(kMaxTestCount, tests->size());
}

BENCHMARK_TEMPLATE(BM_Day, NewDay)->DenseRange(0, kMaxTestCount - 1);
