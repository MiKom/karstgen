#include "config.h"
#include "marchingcubes.h"

#include "gtest/gtest.h"


class ClassifyTest : public testing::Test
{
};

TEST_F(ClassifyTest, FlatSurfaceTest)
{
	EXPECT_EQ(1,1);
}
