#include "advent_of_code/2024/day23/day23.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2024_23>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2024_23)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2024_23>());

}  // namespace advent_of_code
