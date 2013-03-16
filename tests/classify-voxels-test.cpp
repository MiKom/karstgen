#include "config.h"
#include "context.h"
#include "grid.h"
#include "marchingcubes.h"

#include <memory>
#include <iostream>

#include "gtest/gtest.h"
#include "util.h"
#include "common-test.h"

class ClassifyTest : public CommonTest
{
};

TEST_F(ClassifyTest, FlatSurfaceTest)
{
	const int dimLen = 128;
	const int sliceSize = dimLen * dimLen;
	const int gridSize = dimLen * dimLen * dimLen;
	
	const int gridDataSliceSize = (dimLen + 1) * (dimLen + 1);
	const int gridDataSize = (dimLen + 1) * (dimLen + 1) * (dimLen + 1);
	
	uint3 gridDim{dimLen};
	float3 voxelSize{1.0f};
	float3 startPos{0.0f};
	
	cl::CommandQueue queue = ctx->getQueues()[0];
	Grid grid{gridDim, voxelSize, startPos, ctx->getClContext(), queue};
	
	float4 *values = grid.getValues();

	//Set first slice to all -1's
	for(int i=0; i<gridDataSliceSize; i++) {
		values[i] = {-1.0f};
	}
	for(int i=gridDataSliceSize; i<gridDataSize; i++) {
		values[i] = {1.0f};
	}

	grid.copyToDevice();
	
	size_t bufferSize = sizeof(uint) * gridSize;
	
	cl::Buffer voxelVerts(
		ctx->getClContext(),
		CL_MEM_READ_WRITE,
		bufferSize
	);
	cl::Buffer voxelOccupied(
		ctx->getClContext(),
		CL_MEM_READ_WRITE,
		bufferSize
	);
	ctx->getMcProgram()->launchClassifyVoxel(grid, voxelVerts, voxelOccupied, 0.0f);
	
	std::unique_ptr<uint[]> voxelVertsResult{new uint[gridSize]};
	std::unique_ptr<uint[]> voxelOccupiedResult{new uint[gridSize]};
	
	queue.enqueueReadBuffer(voxelVerts, CL_TRUE, 0, bufferSize, voxelVertsResult.get());
	queue.enqueueReadBuffer(voxelOccupied, CL_TRUE, 0, bufferSize, voxelOccupiedResult.get());
	
	std::unique_ptr<uint[]> voxelVertsRef{new uint[gridSize]};
	std::unique_ptr<uint[]> voxelOccupiedRef{new uint[gridSize]};
	
	for(int i=0; i<sliceSize; i++) {
		voxelVertsRef[i] = 6;
		voxelOccupiedRef[i] = 1;
	}
	for(int i=sliceSize; i<gridSize; i++) {
		voxelVertsRef[i] = 0;
		voxelOccupiedRef[i] = 0;
	}
	bool voxelVertsTestResult = arrays_equal(voxelVertsResult.get(), voxelVertsRef.get(), gridSize);
	bool voxelOccupiedTestResult = arrays_equal(voxelOccupiedResult.get(), voxelOccupiedRef.get(), gridSize);
	EXPECT_TRUE(voxelVertsTestResult && voxelOccupiedTestResult);
}
