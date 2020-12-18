#include "advent_of_code/file_benchmark.h"
#include "advent_of_code/new_day_path/new_day.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

constexpr int kMaxTestCount = 2;

TEST(DayBenchmarkTest, Consistency) {
  absl::StatusOr<int> test_count = FileBenchmarkTestCount();
  ASSERT_TRUE(test_count.ok()) << test_count.status().message();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, NewDay)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
}  // namespace advent_of_code
