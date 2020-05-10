#include "gtest/gtest.h"

#include "Update.h"

TEST(Update, TestUpdate)
{
  DoImmune(3);
  EXPECT_EQ(1, 2);
}
