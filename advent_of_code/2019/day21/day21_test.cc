#include "advent_of_code/2019/day21/day21.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

using DayTest = FileTest<Day21_2019>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
