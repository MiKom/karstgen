#include "config.h"
#include "blob.h"
#include "util.h"

#include <stdexcept>
#include <algorithm>

using namespace std;

static const std::string sPath = "kernels/blob.cl";

Blob::Blob(
	const cl::Context &context,
	const vector<cl::CommandQueue>& commandQueues)
	: AbstractProgram(sPath, context, commandQueues)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
	mNormalsKernel = cl::Kernel(mProgram, "calculateNormalsFromGradient");
	cl::CommandQueue q = commandQueues[0];
	cl::Device dev;
	q.getInfo(CL_QUEUE_DEVICE, &dev);
	dev.getInfo(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, &mConstantBufferSize);
}

void Blob::runBlob(const float4 *const blobs, int nBlobs, Grid &grid) const
{
	cl_int blobsPerRun = mConstantBufferSize / sizeof(blobs[0]);
	for(int i=0; i<nBlobs; i+= blobsPerRun) {
		cl::Buffer blobBuffer;
		ulong partStart = i;
		ulong partSize = std::min(nBlobs - i, blobsPerRun);
		std::cout << "Running " << partSize << " blobs starting at " << partStart << std::endl;
		//mFirstQueue.enqueueWriteBuffer(blobBuffer, CL_TRUE, )
	}
	throw std::runtime_error("Not implemented");
}
