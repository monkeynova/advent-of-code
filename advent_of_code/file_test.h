#ifndef ADVENT_OF_CODE_FILE_TEST_H
#define ADVENT_OF_CODE_FILE_TEST_H

#include "advent_of_code/advent_day.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

namespace advent_of_code {

std::string TestCaseFileName();
std::vector<int> TestCaseLines();

bool TestSingleDay(AdventDay* solver);

template <typename Day>
class FileTest : public ::testing::Test {
 public:
  bool RunTest() {
    Day solver;
    return TestSingleDay(&solver);
  }
};

#endif  //  ADVENT_OF_CODE_FILE_TEST_H
}  // namespace advent_of_code
