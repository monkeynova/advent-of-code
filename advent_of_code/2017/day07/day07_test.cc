#include "advent_of_code/2017/day07/day07.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

using DayTest = FileTest<Day07_2017>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
}  // namespace advent_of_code