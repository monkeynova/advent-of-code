#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/new_day_path/new_day.h"
#include "gmock/gmock.h"

namespace advent_of_code {

constexpr int kMaxTestCount = 5;

TEST(DayBenchmarkTest_NewDay, Consistency) {
  NewDay day;
  absl::StatusOr<int> test_count = FileBenchmarkTestCount(&day);
  ASSERT_TRUE(test_count.ok()) << test_count.status().ToString();
  EXPECT_EQ(kMaxTestCount, *test_count);
}

BENCHMARK_TEMPLATE(BM_Day, NewDay)->DenseRange(0, kMaxTestCount - 1);

}  // namespace advent_of_code
