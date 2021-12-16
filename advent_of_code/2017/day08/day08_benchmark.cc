#include "advent_of_code/2017/day08/day08.h"
#include "advent_of_code/infra/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

constexpr int kMaxTestCount = 4;

TEST(DayBenchmarkTest_2017_08, Consistency) {
  Day_2017_08 day;
  absl::StatusOr<int> test_count = FileBenchmarkTestCount(&day);
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, Day_2017_08)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
}  // namespace advent_of_code
