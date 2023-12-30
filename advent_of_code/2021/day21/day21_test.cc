#include "advent_of_code/2021/day21/day21.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2021_21>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2021_21)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2021_21>());

}  // namespace advent_of_code
