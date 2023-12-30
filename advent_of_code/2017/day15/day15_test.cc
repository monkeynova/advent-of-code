#include "advent_of_code/2017/day15/day15.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2017_15>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2017_15)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2017_15>());

}  // namespace advent_of_code
