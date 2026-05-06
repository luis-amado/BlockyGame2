#include <gtest/gtest.h>
#include "util/MathUtil.h"

TEST(MathUtil, Mod) {
  EXPECT_EQ(MathUtil::Mod(-1, 16), 15);
  EXPECT_EQ(MathUtil::Mod(1, 16), 1);
  EXPECT_EQ(MathUtil::Mod(-16, 16), 0);
  EXPECT_EQ(MathUtil::Mod(-17, 16), 15);
}