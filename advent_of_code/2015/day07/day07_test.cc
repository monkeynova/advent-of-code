#include "advent_of_code/2015/day07/day07.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day07_2015>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }