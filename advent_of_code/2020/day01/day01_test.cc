#include "advent_of_code/2020/day01/day01.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

using DayTest = FileTest<Day_2020_01>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
