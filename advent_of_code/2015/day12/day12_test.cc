#include "advent_of_code/2015/day12/day12.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

using DayTest = FileTest<Day_2015_12>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
