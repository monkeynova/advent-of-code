#include "advent_of_code/2016/day12/day12.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2016_12>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2016_12)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2016_12>());

}  // namespace advent_of_code
