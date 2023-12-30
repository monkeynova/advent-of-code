#include "advent_of_code/2019/day04/day04.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2019_04>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2019_04)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2019_04>());

}  // namespace advent_of_code
