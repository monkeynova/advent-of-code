#include "advent_of_code/2020/day02/day02.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day02_2020>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }