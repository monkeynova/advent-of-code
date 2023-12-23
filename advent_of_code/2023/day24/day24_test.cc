#include "advent_of_code/2023/day24/day24.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

using DayTest = FileTest<Day_2023_24>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
