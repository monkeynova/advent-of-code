#include "advent_of_code/2019/day08/day08.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_2019_08>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_2019_08)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2019_08>());

}  // namespace advent_of_code
