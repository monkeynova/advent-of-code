#include "advent_of_code/2021/day15/day15.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

using DayTest = FileTest<Day_2021_15>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
}  // namespace advent_of_code