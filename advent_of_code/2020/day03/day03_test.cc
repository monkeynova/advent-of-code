#include "advent_of_code/2020/day03/day03.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2020_03>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2020_03)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2020_03>());

}  // namespace advent_of_code
