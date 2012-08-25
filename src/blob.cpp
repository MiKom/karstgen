#include "config.h"
#include "blob.h"
#include "util.h"

#include <iostream>

using namespace std;

static const std::string sPath = "kernels/blob.cl";

Blob::Blob(cl::Context& context, vector<cl::CommandQueue> commandQueues)
	: AbstractProgram(sPath, context, commandQueues)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
}
