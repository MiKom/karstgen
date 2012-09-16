#include "config.h"
#include "grid.h"

Grid::Grid(
	uint3 gridSize,
	float3 voxelSize,
	float3 startPos,
	cl::Context& context
) : 
	mGridSize(gridSize),
	mVoxelSize(voxelSize),
	mStartPos(startPos),
	mContext(context)
{
	size_t len = (mGridSize.x + 1) * (mGridSize.y+1) * (mGridSize.z+1);
	mValues = cl::Buffer(mContext, CL_MEM_READ_WRITE,
	                     sizeof(cl_float)*len);
	mNormals = cl::Buffer(mContext, CL_MEM_READ_WRITE,
	                      sizeof(cl_float3) * len);
}
