#include "config.h"

#include "marchingcubes.h"

using namespace std;

//path of the cl source
static const string sPath = "kernels/marchingcubes.cl";

//Kernel fucntions names
static const char sClassifyVoxelFunc[] = "classifyVoxel";
static const char sCompactVoxelsFunc[] = "compactVoxels";
static const char sScanKernelFunc[] = "scan";
static const char sGenerateTrianglesFunc[] = "generateTriangles";

MarchingCubes::MarchingCubes(
	cl::Context ctx,
	vector<cl::CommandQueue> queues
) : AbstractProgram(sPath, ctx, queues)
{
	mClassifyVoxelKernel = cl::Kernel(mProgram, sClassifyVoxelFunc);
	mCompactVoxelsKernel = cl::Kernel(mProgram, sCompactVoxelsFunc);
	mGenerateTrianglesKernel = cl::Kernel(mProgram, sGenerateTrianglesFunc);
}
