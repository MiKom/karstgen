#include "config.h"
#include "grid.h"

Grid::Grid(
	uint3 gridDim,
	float3 voxelSize,
	float3 startPos,
	cl::Context& context,
	cl::CommandQueue& cq
) : 
	mGridDim(gridDim),
	mVoxelSize(voxelSize),
	mStartPos(startPos),
	mContext(context),
	mCommandQueue(cq),
	mStorage(Storage::HOST)
{
	size_t len = (mGridDim.x + 1) * (mGridDim.y+1) * (mGridDim.z+1);
	
	mValues = new float4[len];
}

Grid::~Grid()
{
	delete[] mValues;
}

void
Grid::copyToDevice()
{
	if(mStorage != Storage::DEVICE) {
		size_t len =(mGridDim.x + 1) * (mGridDim.y+1) * (mGridDim.z+1);
		size_t dataSize = len * sizeof(cl_float4);
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
		size_t len =(mGridDim.x + 1) * (mGridDim.y+1) * (mGridDim.z+1);
		size_t dataSize = len * sizeof(cl_float4);
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
