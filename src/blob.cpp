#include "config.h"
#include "blob.h"
#include "util.h"

#include <stdexcept>

using namespace std;

static const std::string sPath = "kernels/blob.cl";

Blob::Blob(
	const cl::Context &context,
	const vector<cl::CommandQueue>& commandQueues)
	: AbstractProgram(sPath, context, commandQueues)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
	mNormalsKernel = cl::Kernel(mProgram, "calculateNormalsFromGradient");
}

void Blob::runBlob(const float4 *const blobs, int nBlobs, Grid &grid) const
{
	throw std::runtime_error("Not implemented");
}
