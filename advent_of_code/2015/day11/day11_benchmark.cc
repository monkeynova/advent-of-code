#include "advent_of_code/2015/day11/day11.h"
#include "advent_of_code/infra/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest, Consistency) {
  Day_2015_11 day;
  absl::StatusOr<int> test_count = FileBenchmarkTestCount(&day);
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day_2015_11)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
