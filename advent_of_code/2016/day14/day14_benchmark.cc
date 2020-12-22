#include "advent_of_code/2016/day14/day14.h"
#include "advent_of_code/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

constexpr int kMaxTestCount = 4;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<int> test_count = FileBenchmarkTestCount();
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day14_2016)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
}  // namespace advent_of_code
