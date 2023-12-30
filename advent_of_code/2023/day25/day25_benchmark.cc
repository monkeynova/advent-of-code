#include "advent_of_code/2023/day25/day25.h"
#include "advent_of_code/infra/file_benchmark.h"
#include "gmock/gmock.h"

namespace advent_of_code {

BENCHMARK_TEMPLATE(BM_Day, Day_2023_25)
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_2023_25>());

}  // namespace advent_of_code
