#include "config.h"
#include "grid.h"

Grid::Grid(
	uint3 gridDim,
	float3 voxelSize,
	float3 startPos,
	cl::Context& context
) : 
	mGridDim(gridDim),
	mVoxelSize(voxelSize),
	mStartPos(startPos),
	mContext(context),
	mStorage(Storage::HOST)
{
	size_t len = (mGridDim.x + 1) * (mGridDim.y+1) * (mGridDim.z+1);
	
	mValues = new float3[len];
	mNormals = new float3[len];
	/*
	mValuesBuffer = cl::Buffer(mContext, CL_MEM_READ_WRITE,
	                     sizeof(cl_float)*len);
	mNormalsBuffer = cl::Buffer(mContext, CL_MEM_READ_WRITE,
	                      sizeof(cl_float3) * len);
	*/
}

Grid::~Grid()
{
	delete[] mValues;
	delete[] mNormals;
}

Grid::copyToDevice()
{
	//TODO: implement
	if(storage != Storage::DEVICE) {
		
	} else {
		return;
	}
}

Grid::copyToHost()
{
	//TODO: implement
}
