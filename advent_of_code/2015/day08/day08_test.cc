#include "advent_of_code/2015/day08/day08.h"

#include "advent_of_code/file_test.h"
#include "gmock/gmock.h"

using DayTest = FileTest<Day08_2015>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }