#include "advent_of_code/2017/day13/day13.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2017_13>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2017_13)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2017_13>());

}  // namespace advent_of_code
