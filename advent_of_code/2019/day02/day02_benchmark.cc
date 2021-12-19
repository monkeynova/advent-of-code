#include "advent_of_code/2019/day02/day02.h"
#include "advent_of_code/infra/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest_2019_02, Consistency) {
  Day_2019_02 day;
  absl::StatusOr<int> test_count = FileBenchmarkTestCount(&day);
  ASSERT_TRUE(test_count.ok()) << test_count.status().ToString();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day_2019_02)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
