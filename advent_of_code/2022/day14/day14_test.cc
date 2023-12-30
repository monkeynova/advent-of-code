#include "advent_of_code/2022/day14/day14.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2022_14>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2022_14)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2022_14>());

}  // namespace advent_of_code
