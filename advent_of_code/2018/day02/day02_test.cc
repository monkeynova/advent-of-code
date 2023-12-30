#include "advent_of_code/2018/day02/day02.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2018_02>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2018_02)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2018_02>());

}  // namespace advent_of_code
