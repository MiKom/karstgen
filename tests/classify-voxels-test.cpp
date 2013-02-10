#include "config.h"
#include "context.h"
#include "grid.h"
#include "marchingcubes.h"

#include "gtest/gtest.h"
#include "common-test.h"

class ClassifyTest : public CommonTest
{
};

TEST_F(ClassifyTest, FlatSurfaceTest)
{
	EXPECT_EQ(1,1);
}
