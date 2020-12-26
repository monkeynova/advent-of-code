#include "advent_of_code/2017/day07/day07.h"
#include "advent_of_code/infra/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<int> test_count = FileBenchmarkTestCount();
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day07_2017)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
}  // namespace advent_of_code
