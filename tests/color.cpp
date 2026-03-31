#include <gtest/gtest.h>
#include "util/Color.h"
#include "util/Logging.h"

TEST(ColorTest, CorrectColorGenerated) {
  Color reddish("#992238");
  glm::vec4 rgba = reddish.GetRGBA();
  EXPECT_EQ(rgba.r, 153 / 255.f);
  EXPECT_EQ(rgba.g, 34 / 255.f);
  EXPECT_EQ(rgba.b, 56 / 255.f);
}