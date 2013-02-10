#include "config.h"
#include "grid.h"

Grid::Grid(
	uint3 gridDim,
	float3 voxelSize,
	float3 startPos,
	cl::Context& context,
	cl::CommandQueue& cq
) : 
	mGridDim{gridDim},
	mVoxelSize{voxelSize},
	mStartPos{startPos},
	mContext{context},
	mCommandQueue{cq},
	mStorage{Storage::HOST},
	mValues{new float4[getFlatDataSize(gridDim)]}
{ }

Grid::~Grid()
{
	delete[] mValues;
}

unsigned int
Grid::getFlatDataSize(const uint3& gridDim)
{
	return (gridDim.x + 1) * (gridDim.y + 1) * (gridDim.z + 1);
}

void
Grid::copyToDevice()
{
	if(mStorage != Storage::DEVICE) {
		size_t dataSize = getFlatDataSize(mGridDim) * sizeof(cl_float4);
		mValuesBuffer = cl::Buffer(mContext, CL_MEM_READ_WRITE,
		                           dataSize);
		mCommandQueue.enqueueWriteBuffer(mValuesBuffer, CL_TRUE,
		                                 0, dataSize, mValues);
		mStorage = Storage::DEVICE;
	} else {
		//Data already on the device
		return;
	}
}

void
Grid::copyToHost()
{
	if(mStorage != Storage::HOST) {
		size_t dataSize = getFlatDataSize(mGridDim) * sizeof(cl_float4);
		mCommandQueue.enqueueReadBuffer(mValuesBuffer, CL_TRUE, 0,
		                                dataSize, mValues);
		//Losing reference to Device buffer so it can get deallocated
		mValuesBuffer = cl::Buffer();
		mStorage = Storage::HOST;
	} else {
		//Data already on host
		return;
	}
}
