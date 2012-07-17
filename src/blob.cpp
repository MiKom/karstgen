#include "config.h"
#include "blob.h"
#include "util.h"

#include <iostream>

using namespace std;

static const std::string sPath = "kernels/blob.cl";

Blob::Blob(cl::Context& context)
	: AbstractProgram(sPath, context)
{
	mBlobValKernel = cl::Kernel(mProgram, "blobValue");
}
