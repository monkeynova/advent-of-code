#include "advent_of_code/2019/day23/day23.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day23_2019>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }