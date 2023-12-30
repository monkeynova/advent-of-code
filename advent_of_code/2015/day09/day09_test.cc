#include "advent_of_code/2015/day09/day09.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2015_09>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2015_09)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2015_09>());

}  // namespace advent_of_code
