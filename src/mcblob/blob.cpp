#include "config.h"
#include "grid.h"
#include "blob.h"
#include "util.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

static const std::string sPath = "kernels/blob.cl";

static const int BLOB_THREADS_PER_WG = 0;
static const bool BLOB_USE_ALL_CARDS = false;

Blob::Blob(
	const cl::Context &context,
	const vector<cl::CommandQueue>& commandQueues)
	: AbstractProgram(sPath, context, commandQueues)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
	cl::CommandQueue q = commandQueues[0];
	cl::Device dev;
	q.getInfo(CL_QUEUE_DEVICE, &dev);
	dev.getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &mConstantBufferSize);
}

/**
  This method adds an array of blobs to the scalar field
  \param blobs array of blobs to be added. Positions of the blobs are
  kept in x,y and z components and magnitude (size) of the blob is read
  from w component.
  \param nBlobs length of blobs array
  \param grid grid to which blob values will be added
  */
void Blob::runBlob(const float4 *const blobs, int nBlobs, Grid &grid)
{
	cl_int blobsPerRun = mConstantBufferSize / sizeof(blobs[0]);
	grid.copyToDevice();

	cl::Buffer blobBuffer = cl::Buffer(
		mContext,
		CL_MEM_READ_ONLY,
		mConstantBufferSize
	);
	
	for(int i=0; i<nBlobs; i+= blobsPerRun) {
		size_t partStart = i;
		size_t partSize = std::min(nBlobs - i, blobsPerRun);
		
		mFirstQueue.enqueueWriteBuffer(
			blobBuffer,
			CL_TRUE,
			0,
			partSize * sizeof(float4),
			blobs + partStart
		);
		
		cl_int nPoints = (grid.getGridSize().x + 1) *
		                 (grid.getGridSize().y + 1) *
		                 (grid.getGridSize().z + 1);
		
		uint arg = 0;
		mBlobValKernel.setArg(arg++, grid.getStartPos());
		mBlobValKernel.setArg(arg++, grid.getGridSize());
		mBlobValKernel.setArg(arg++, grid.getVoxelSize());
		mBlobValKernel.setArg(arg++, blobBuffer);
		mBlobValKernel.setArg(arg++, (int) partSize);
		mBlobValKernel.setArg(arg++, grid.getValuesBuffer());
		mBlobValKernel.setArg(arg++, nPoints);
		
		if(BLOB_USE_ALL_CARDS) {
			run1DKernelMultipleQueues(
				mBlobValKernel,
				mCommandQueues,
				nPoints,
				BLOB_THREADS_PER_WG
			);
		} else {
			run1DKernelSingleQueue(
				mBlobValKernel,
				mCommandQueues[0],
				nPoints,
				BLOB_THREADS_PER_WG
			);
		}
		
	}
}
