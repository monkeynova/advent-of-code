#include "advent_of_code/2015/day15/day15.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day15_2015>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }
