#include "config.h"
#include "context.h"
#include "grid.h"
#include "marchingcubes.h"
#include "util.h"

#include <memory>
#include <iostream>

#include "gtest/gtest.h"
#include "common-test.h"

using namespace std;

class CompactVoxelsTest : public CommonTest
{
protected:
	void cpu_compact(
		uint *compactedArray,
		uint *voxelOccupiedArray,
		uint *voxelOccupiedScanArray,
		uint numVoxels
	){
		for(int i=0; i<numVoxels; i++) {
			if(voxelOccupiedArray[i] == 1) {
				compactedArray[voxelOccupiedScanArray[i]] = i;
			}
		}
	}
};

TEST_F(CompactVoxelsTest, CompactFlatSurfaceTest)
{
	const int dimLen = 64;
	const int sliceSize = dimLen * dimLen;
	const int gridSize = dimLen * dimLen * dimLen;
	
	cl::CommandQueue queue = ctx->getQueues()[0];
	

	//Set first slice to all -1's
	unique_ptr<uint[]> voxelOccupiedPtr{new uint[gridSize]};
	unique_ptr<uint[]> voxelOccupiedScanPtr{new uint[gridSize]};
	
	uint *voxelOccupied = voxelOccupiedPtr.get();
	memset(voxelOccupied, 0, sizeof(uint) * gridSize);
	uint *voxelOccupiedScan = voxelOccupiedScanPtr.get();
	
	//Setting whole first slice to contain voxels
	for(int i=0; i<sliceSize; i++) {
		voxelOccupied[i] = 1;
	}
	
	cpu_scan<uint>(voxelOccupied, voxelOccupiedScan, gridSize);
	
	cl::Buffer voxelOccupiedDev{
		ctx->getClContext(),
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(uint) * gridSize,
		voxelOccupied
	};
	cl::Buffer voxelOccupiedScanDev{
		ctx->getClContext(),
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(uint) * gridSize,
		voxelOccupiedScan
	};
	
	uint activeVoxels = voxelOccupiedScan[gridSize - 1] + voxelOccupied[gridSize - 1];
	
	cl::Buffer compactedVoxelOccupiedDev{
		ctx->getClContext(),
		CL_MEM_WRITE_ONLY,
		sizeof(uint) * activeVoxels
	};

	unique_ptr<uint[]> compactedVoxelOccupiedPtr{new uint[activeVoxels]};
	uint* compactedVoxelArray = compactedVoxelOccupiedPtr.get();
	
	ctx->getMcProgram()->launchCompactVoxels(
		compactedVoxelOccupiedDev,
		voxelOccupiedDev,
		voxelOccupiedScanDev,
		gridSize
	);
	cl::CommandQueue q = ctx->getQueues()[0];
	q.enqueueReadBuffer(compactedVoxelOccupiedDev, CL_TRUE, 0, sizeof(uint) * activeVoxels, compactedVoxelArray);
	
	//Computing reference result
	unique_ptr<uint[]> refCompactedArray{new uint[activeVoxels]};
	cpu_compact(refCompactedArray.get(), voxelOccupied, voxelOccupiedScan, activeVoxels);
	
	bool testResult = arrays_equal<uint>(compactedVoxelArray, refCompactedArray.get(), activeVoxels);
	EXPECT_TRUE(testResult);
}
