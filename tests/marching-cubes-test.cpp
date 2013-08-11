#include "config.h"
#include "context.h"
#include "grid.h"
#include "marchingcubes.h"

#include <memory>
#include <iostream>

#include "gtest/gtest.h"
#include "common-test.h"

class MarchingCubesTest : public CommonTest
{
};

TEST_F(MarchingCubesTest, FlatSurfaceTest)
{
	const int dimLen = 64;
	const int sliceSize = dimLen * dimLen;
	const int gridSize = dimLen * dimLen * dimLen;
	
	const int gridDataSliceSize = (dimLen + 1) * (dimLen + 1);
	const int gridDataSize = (dimLen + 1) * (dimLen + 1) * (dimLen + 1);
	
	uint3 gridDim{dimLen};
	float3 voxelSize{1.0f};
	float3 startPos{0.0f};
	
	cl::CommandQueue queue = ctx->getQueues()[0];
	Grid grid{gridDim, voxelSize, startPos, ctx->getClContext(), queue, ctx->getMemsetKernel()};
	
	float4 *values = grid.getValues();

	//Set first slice to all -1's
	for(int i=0; i<gridDataSliceSize; i++) {
		values[i] = {-1.0f};
	}
	for(int i=gridDataSliceSize; i<gridDataSize; i++) {
		values[i] = {1.0f};
	}

	grid.copyToDevice();
	
	MCMesh result = ctx->getMcProgram()->compute(grid, 0.0f);
	
	EXPECT_TRUE( result.verts.size() == 64*64*6);
}
