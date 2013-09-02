#include "config.h"
#include "grid.h"
#include "util.h"

/**
  \param gridDim dimension of grid, i.e. number of voxels in each dimension
  \param voxelSize size of singe voxel
  \param startPos starting position of the grid. Start position is place in
    smallest x, y and z position
  \param context OpenCL context within which this grid will operate
  \param cq OpenCL command queue (in the same context as context
  parameter) that will be used to move data back and forth between the
  devices.
  */
Grid::Grid(
	uint3 gridDim,
	float3 voxelSize,
	float3 startPos,
	cl::Context& context,
	cl::CommandQueue& cq,
	cl::Kernel& memSetKernel
) : 
	mGridDim{gridDim},
	mVoxelSize{voxelSize},
	mStartPos{startPos},
	mContext{context},
	mCommandQueue{cq},
	mMemSetKernel{memSetKernel},
	mStorage{Storage::HOST},
	mValues{new float4[getFlatDataSize(gridDim)]}
{ }

Grid::~Grid()
{
	delete[] mValues;
}

/**
  This function calculates the number of data point on the grid that
  will be needed to keep the grid data. Basically, for x*y*z sized grid
  (in voxels in each dimension) you need (x+1) * (y+1) * (z+1) data
  points. This is a utility function to compute that.
  */
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

/**
 * @brief clear grid data to specified value
 *
 * @param val value to which all data in grid will be set
 */
void
Grid::clear(float val)
{
	if(mStorage == Storage::HOST) {
		std::fill_n(mValues, getFlatDataSize(mGridDim), val);
	} else {
		unsigned int i = 0;
		mMemSetKernel.setArg(i++, val);
		mMemSetKernel.setArg(i++, mValuesBuffer);
		
		run1DKernelSingleQueue(
			mMemSetKernel,
			mCommandQueue,
			getFlatDataSize(mGridDim)
		);
	}
}
