#include "advent_of_code/new_day_path/new_day.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<NewDay>()); }

BENCHMARK_TEMPLATE(BM_Day, NewDay)
    ->DenseRange(0, FileBenchmarkMaxIdx<NewDay>());

}  // namespace advent_of_code
