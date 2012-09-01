#include "config.h"
#include "tables.h"

#include "marchingcubes.h"

using namespace std;

//path of the cl source
static const string sPath = "kernels/marchingcubes.cl";

//Kernel fucntions names
static const char sClassifyVoxelFunc[] = "classifyVoxel";
static const char sCompactVoxelsFunc[] = "compactVoxels";
static const char sGenerateTrianglesFunc[] = "generateTriangles";

MarchingCubes::MarchingCubes(
	const cl::Context ctx,
	const vector<cl::CommandQueue>& queues
) : AbstractProgram(sPath, ctx, queues)
{
	//initializing kernels
	mClassifyVoxelKernel = cl::Kernel(mProgram, sClassifyVoxelFunc);
	mCompactVoxelsKernel = cl::Kernel(mProgram, sCompactVoxelsFunc);
	mGenerateTrianglesKernel = cl::Kernel(mProgram, sGenerateTrianglesFunc);
	
	//initializing textures with tables for Marching Cubes
	cl::ImageFormat format;
	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_UNSIGNED_INT8;
	mTriangleTable = cl::Image2D(ctx,
	                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
	                             format,
	                             16,
	                             256,
	                             0,
	                             (void*) mcTriangleTable);
	
	mNumVertsTable = cl::Image2D(ctx,
	                             CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
	                             format,
	                             256,
	                             1,
	                             0,
	                             (void*) mcNumVertsTable);
}


