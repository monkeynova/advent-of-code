#include "advent_of_code/2017/knot_hash.h"

#include "gtest/gtest.h"

namespace advent_of_code {

TEST(KnotHash, Day10) {
  const char kTmpStr[] = {3, 4, 1, 5, '\0'};
  std::vector<unsigned char> loop(5);
  for (int i = 0; i < loop.size(); ++i) loop[i] = i;
  KnotHash().RunLoop(kTmpStr, &loop);
  EXPECT_EQ(loop[0] * loop[1], 12);
}

TEST(KnotHash, KnownHash) {
  EXPECT_EQ(KnotHash().DigestHex("AoC 2017"),
            "33efeb34ea91902bb2f59c9920caa6cd");
}

}  // namespace advent_of_code