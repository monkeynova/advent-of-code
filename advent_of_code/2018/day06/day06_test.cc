#include "advent_of_code/2018/day06/day06.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2018_06>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2018_06)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2018_06>());

}  // namespace advent_of_code
