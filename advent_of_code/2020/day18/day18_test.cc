#include "advent_of_code/2020/day18/day18.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day18_2020>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }
