#include "advent_of_code/2018/day05/day05.h"

#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

namespace advent_of_code {

using DayTest = FileTest<Day05_2018>;

TEST_F(DayTest, FileBasedTest) { EXPECT_TRUE(RunTest()); }

}  // namespace advent_of_code
}  // namespace advent_of_code
