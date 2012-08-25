#include "config.h"
#include "blob.h"
#include "util.h"

using namespace std;

static const std::string sPath = "kernels/blob.cl";

Blob::Blob(
	const cl::Context &context,
	const vector<cl::CommandQueue>& commandQueues)
	: AbstractProgram(sPath, context, commandQueues)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
}
