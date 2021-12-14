#ifndef ADVENT_OF_CODE_INFRA_FILE_TEST_H
#define ADVENT_OF_CODE_INFRA_FILE_TEST_H

#include "advent_of_code/advent_day.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

namespace advent_of_code {

bool TestSingleDay(AdventDay* day);

template <typename Day>
class FileTest : public ::testing::Test {
 public:
  bool RunTest() {
    Day day;
    return TestSingleDay(&day);
  }
};

}  // namespace advent_of_code

#endif  //  ADVENT_OF_CODE_INFRA_FILE_TEST_H
