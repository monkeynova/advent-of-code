#include "advent_of_code/2019/day7/day7.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day7_2019>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }