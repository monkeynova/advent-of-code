#ifndef ADVENT_OF_CODE_INFRA_FILE_TEST_H
#define ADVENT_OF_CODE_INFRA_FILE_TEST_H

#include "absl/log/log.h"
#include "advent_of_code/advent_day.h"
#include "gtest/gtest.h"

namespace advent_of_code {

bool TestSingleDay(AdventDay* day);

class FileTest_Day : public ::testing::Test {
 public:
  template <typename Day>
  bool RunTest() {
    Day day;
    return TestSingleDay(&day);
  }
};


}  // namespace advent_of_code

#endif  //  ADVENT_OF_CODE_INFRA_FILE_TEST_H
