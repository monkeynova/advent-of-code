#include "advent_of_code/2015/day03/day03.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day03_2015>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }