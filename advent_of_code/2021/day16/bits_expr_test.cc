#include "advent_of_code/2021/day16/bits_expr.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace advent_of_code {

MATCHER_P(IsOkAndHolds, matcher, "") {
  if (!arg.ok()) {
    *result_listener << "status is not OK";
    return false;
  }
  return ExplainMatchResult(matcher, *arg, result_listener);
}

template <typename Matcher>
auto SumVersions(Matcher m) {
  return testing::Property("SumVersions", &BitsExpr::SumVersions, std::move(m));
}

template <typename Matcher>
auto DebugString(Matcher m) {
  return testing::Property("DebugString", &BitsExpr::DebugString, std::move(m));
}

template <typename Matcher>
auto Evaluate(Matcher m) {
  return testing::Property("Evaluate", &BitsExpr::Evaluate, std::move(m));
}

TEST(BitsExpr, SumVersions) {
  EXPECT_THAT(BitsExpr::Parse("D2FE28"), IsOkAndHolds(SumVersions(6)));
  EXPECT_THAT(BitsExpr::Parse("38006F45291200"), IsOkAndHolds(SumVersions(9)));
  EXPECT_THAT(BitsExpr::Parse("8A004A801A8002F478"),
              IsOkAndHolds(SumVersions(16)));
  EXPECT_THAT(BitsExpr::Parse("620080001611562C8802118E34"),
              IsOkAndHolds(SumVersions(12)));
  EXPECT_THAT(BitsExpr::Parse("C0015000016115A2E0802F182340"),
              IsOkAndHolds(SumVersions(23)));
  EXPECT_THAT(BitsExpr::Parse("A0016C880162017C3686B18A3D4780"),
              IsOkAndHolds(SumVersions(31)));
}

TEST(BitsExpr, Evaluate) {
  EXPECT_THAT(BitsExpr::Parse("C200B40A82"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(3))));
  EXPECT_THAT(BitsExpr::Parse("04005AC33890"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(54))));
  EXPECT_THAT(BitsExpr::Parse("880086C3E88112"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(7))));
  EXPECT_THAT(BitsExpr::Parse("CE00C43D881120"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(9))));
  EXPECT_THAT(BitsExpr::Parse("D8005AC2A8F0"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(1))));
  EXPECT_THAT(BitsExpr::Parse("F600BC2D8F"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(0))));
  EXPECT_THAT(BitsExpr::Parse("9C005AC2F8F0"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(0))));
  EXPECT_THAT(BitsExpr::Parse("9C0141080250320F1802104A08"),
              IsOkAndHolds(Evaluate(IsOkAndHolds(1))));
}

TEST(BitsExpr, DebugString) {
  EXPECT_THAT(BitsExpr::Parse("D2FE28"),
              IsOkAndHolds(DebugString("+-ver=6\n"
                                       "+-type=Literal\n"
                                       "  +-literal=2021\n")));
  EXPECT_THAT(BitsExpr::Parse("38006F45291200"),
              IsOkAndHolds(DebugString("+-ver=1\n"
                                       "+-type=Less\n"
                                       "  +-ver=6\n"
                                       "  +-type=Literal\n"
                                       "    +-literal=10\n "
                                       " +-ver=2\n"
                                       "  +-type=Literal\n"
                                       "    +-literal=20\n")));
  EXPECT_THAT(BitsExpr::Parse("8A004A801A8002F478"),
              IsOkAndHolds(DebugString("+-ver=4\n"
                                       "+-type=Min\n"
                                       "  +-ver=1\n"
                                       "  +-type=Min\n"
                                       "    +-ver=5\n"
                                       "    +-type=Min\n"
                                       "      +-ver=6\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=15\n")));
  EXPECT_THAT(BitsExpr::Parse("620080001611562C8802118E34"),
              IsOkAndHolds(DebugString("+-ver=3\n"
                                       "+-type=Sum\n"
                                       "  +-ver=0\n"
                                       "  +-type=Sum\n"
                                       "    +-ver=0\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=10\n"
                                       "    +-ver=5\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=11\n"
                                       "  +-ver=1\n"
                                       "  +-type=Sum\n"
                                       "    +-ver=0\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=12\n"
                                       "    +-ver=3\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=13\n")));
  EXPECT_THAT(BitsExpr::Parse("C0015000016115A2E0802F182340"),
              IsOkAndHolds(DebugString("+-ver=6\n"
                                       "+-type=Sum\n"
                                       "  +-ver=0\n"
                                       "  +-type=Sum\n"
                                       "    +-ver=0\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=10\n"
                                       "    +-ver=6\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=11\n"
                                       "  +-ver=4\n"
                                       "  +-type=Sum\n"
                                       "    +-ver=7\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=12\n"
                                       "    +-ver=0\n"
                                       "    +-type=Literal\n"
                                       "      +-literal=13\n")));
  EXPECT_THAT(BitsExpr::Parse("A0016C880162017C3686B18A3D4780"),
              IsOkAndHolds(DebugString("+-ver=5\n"
                                       "+-type=Sum\n"
                                       "  +-ver=1\n"
                                       "  +-type=Sum\n"
                                       "    +-ver=3\n"
                                       "    +-type=Sum\n"
                                       "      +-ver=7\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=6\n"
                                       "      +-ver=6\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=6\n"
                                       "      +-ver=5\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=12\n"
                                       "      +-ver=2\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=15\n"
                                       "      +-ver=2\n"
                                       "      +-type=Literal\n"
                                       "        +-literal=15\n")));
}

}  //  namespace advent_of_code