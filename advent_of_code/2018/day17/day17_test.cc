#include "advent_of_code/2018/day17/day17.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2018_17>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2018_17)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2018_17>());

}  // namespace advent_of_code
