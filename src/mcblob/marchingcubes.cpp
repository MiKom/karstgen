#include "config.h"
#include "tables.h"

#include "util.h"
#include "grid.h"
#include "scan.h"
#include "marchingcubes.h"

#include <memory>

using namespace std;

//path of the cl source
static const string sPath = "kernels/marchingcubes.cl";

//Kernel fucntions names
static const char sClassifyVoxelFunc[] = "classifyVoxel";
static const char sCompactVoxelsFunc[] = "compactVoxels";
static const char sGenerateTrianglesFunc[] = "generateTriangles";

//Constants
static const int CLASSIFY_VOXELS_THREADS_PER_WG = 128;
static const bool CLASSIFY_VOXELS_USE_ALL_CARDS = false;

static const int COMPACT_VOXELS_THREADS_PER_WG = 128;
static const bool COMPACT_VOXELS_USE_ALL_CARDS = false;

static const int GENERATE_TRIANGLES_THREADS_PER_WG = 32;
static const int GENERATE_TRIANGLES_USE_ALL_CARDS = false;

/**
  \param scan pointer to object storing initialized scan program object
*/
MarchingCubes::MarchingCubes(
	const cl::Context ctx,
	const vector<cl::CommandQueue>& queues,
	Scan *scan) : AbstractProgram(sPath, ctx, queues), mScanOp(scan)
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

void MarchingCubes::launchClassifyVoxel(
	const Grid& grid,
	cl::Buffer voxelVerts,
	cl::Buffer voxelOccupied,
	float isoValue
)
{
	uint3 gridSize = grid.getGridSize();
	unsigned int numVoxels = gridSize.x * gridSize.y * gridSize.z;
	unsigned int i = 0;
	mClassifyVoxelKernel.setArg(i++, grid.getValuesBuffer());
	mClassifyVoxelKernel.setArg(i++, voxelVerts);
	mClassifyVoxelKernel.setArg(i++, voxelOccupied);
	mClassifyVoxelKernel.setArg(i++, grid.getGridSize());
	mClassifyVoxelKernel.setArg(i++, grid.getVoxelSize());
	mClassifyVoxelKernel.setArg(i++, isoValue);
	mClassifyVoxelKernel.setArg(i++, numVoxels);
	mClassifyVoxelKernel.setArg(i++, mNumVertsTable);

	if(CLASSIFY_VOXELS_USE_ALL_CARDS) {
		run1DKernelMultipleQueues(
			mClassifyVoxelKernel,
			mCommandQueues,
			numVoxels,
			CLASSIFY_VOXELS_THREADS_PER_WG
		);
	} else {
		run1DKernelSingleQueue(
			mClassifyVoxelKernel,
			mCommandQueues[0],
			numVoxels,
			CLASSIFY_VOXELS_THREADS_PER_WG
		);
	}
}

void MarchingCubes::launchCompactVoxels(
	cl::Buffer compVoxelArray,
	cl::Buffer voxelOccupied,
	cl::Buffer voxelOccupiedScan,
	unsigned int numVoxels)
{
	int i=0;
	mCompactVoxelsKernel.setArg(i++, compVoxelArray);
	mCompactVoxelsKernel.setArg(i++, voxelOccupied);
	mCompactVoxelsKernel.setArg(i++, voxelOccupiedScan);
	mCompactVoxelsKernel.setArg(i++, numVoxels);
	
	if(COMPACT_VOXELS_USE_ALL_CARDS) {
		run1DKernelMultipleQueues(
			mCompactVoxelsKernel,
			mCommandQueues,
			numVoxels,
			COMPACT_VOXELS_THREADS_PER_WG
		);
	} else {
		run1DKernelSingleQueue(
			mCompactVoxelsKernel,
			mCommandQueues[0],
			numVoxels,
			COMPACT_VOXELS_THREADS_PER_WG
		);
	}
}

void MarchingCubes::launchGenerateTriangles(
	cl::Buffer pos,
	cl::Buffer norm,
	cl::Buffer compVoxelArray,
	cl::Buffer numVertsScanned,
	float isoValue,
	unsigned int activeVoxels,
	unsigned int maxVerts,
	const Grid& grid)
{
	int i=0;
	mGenerateTrianglesKernel.setArg(i++, pos);
	mGenerateTrianglesKernel.setArg(i++, norm);
	mGenerateTrianglesKernel.setArg(i++, grid.getValuesBuffer());
	mGenerateTrianglesKernel.setArg(i++, compVoxelArray);
	mGenerateTrianglesKernel.setArg(i++, numVertsScanned);
	mGenerateTrianglesKernel.setArg(i++, grid.getGridSize());
	mGenerateTrianglesKernel.setArg(i++, grid.getVoxelSize());
	mGenerateTrianglesKernel.setArg(i++, grid.getStartPos());
	mGenerateTrianglesKernel.setArg(i++, isoValue);
	mGenerateTrianglesKernel.setArg(i++, activeVoxels);
	mGenerateTrianglesKernel.setArg(i++, maxVerts);
	mGenerateTrianglesKernel.setArg(i++, mNumVertsTable);
	mGenerateTrianglesKernel.setArg(i++, mTriangleTable);
	if(GENERATE_TRIANGLES_USE_ALL_CARDS) {
		run1DKernelMultipleQueues(
			mGenerateTrianglesKernel,
			mCommandQueues,
			activeVoxels,
			GENERATE_TRIANGLES_THREADS_PER_WG
		);
	} else {
		run1DKernelSingleQueue(
			mCompactVoxelsKernel,
			mCommandQueues[0],
			activeVoxels,
			GENERATE_TRIANGLES_THREADS_PER_WG
		);
	}
}

/**
  This function computes triangle mesh from scalar field described
  by grid. Value that will be treated as the frontier of the isosurface
  is passed in isoValue parameter.
  
  \param grid scalar field which describes isosurface
  \param isoValue value that will be treated as a frontier of the
  surface
  \return simple structure containing pointers to vector of vertices
  (triplets of coordinates) and normals (triplets of coordinates as well)
*/
MCMesh MarchingCubes::compute(Grid &grid, float isoValue)
{
	MCMesh ret = { vector<float3>(), vector<float3>()};
	grid.copyToDevice();
	uint3 gridSize = grid.getGridSize();
	
	unsigned int numVoxels = gridSize.x * gridSize.y * gridSize.z;
	cl::Buffer voxelVerts = cl::Buffer(
		mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * numVoxels);
	
	cl::Buffer voxelOccupied = cl::Buffer(
		mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * numVoxels);
	
	launchClassifyVoxel(grid, voxelVerts, voxelOccupied, isoValue);
	
	cl::Buffer voxelOccupiedScan = cl::Buffer(
		mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * numVoxels);
	mScanOp->compute(voxelOccupied, voxelOccupiedScan, numVoxels);
	
	//Reading total number of non-empty voxels
	uint lastElement, lastScanElement;
	cl::CommandQueue q = mCommandQueues[0];
	q.enqueueReadBuffer(
		voxelOccupied,
		CL_TRUE,
		(numVoxels-1) * sizeof(uint),
		sizeof(uint),
		&lastElement
	);
	q.enqueueReadBuffer(
		voxelOccupiedScan,
		CL_TRUE,
		(numVoxels-2) * sizeof(uint),
		sizeof(uint),
		&lastScanElement
	);
	int activeVoxels = lastElement + lastScanElement;
	
	if(activeVoxels == 0) {
		return ret;
	}
	
	// Compacting array of occupied voxels
	cl::Buffer compactedVoxelArray = cl::Buffer(
		mContext, CL_MEM_READ_WRITE, sizeof(uint) * activeVoxels);
	launchCompactVoxels(compactedVoxelArray,
		voxelOccupied, voxelOccupiedScan, numVoxels);
	
	//Reading total number of vertices to generate
	cl::Buffer voxelVertsScan = cl::Buffer(mContext, CL_MEM_READ_WRITE,
		sizeof(cl_uint) * numVoxels);
	mScanOp->compute(voxelVerts, voxelVertsScan, numVoxels);
	q.enqueueReadBuffer(
		voxelVerts,
		CL_TRUE,
		(numVoxels - 1) * sizeof(uint),
		sizeof(uint),
		&lastElement
	);
	q.enqueueReadBuffer(
		voxelVertsScan,
		CL_TRUE,
		(numVoxels - 2) * sizeof(uint),
		sizeof(uint),
		&lastScanElement
	);
	int totalVerts = lastElement + lastScanElement;
	//this is not needed anymore
	voxelVerts = cl::Buffer();
	cl::Buffer normals = cl::Buffer(
		mContext, CL_MEM_WRITE_ONLY, sizeof(cl_float4) * totalVerts);
	cl::Buffer verts = cl::Buffer(
		mContext, CL_MEM_WRITE_ONLY, sizeof(cl_float4) * totalVerts);
	
	launchGenerateTriangles(
		verts,
		normals,
		compactedVoxelArray,
		voxelVertsScan,
		isoValue,
		activeVoxels,
		totalVerts,
		grid
	);
	
	unique_ptr<float3[]> hVertices{new float3[totalVerts]};
	unique_ptr<float3[]> hNormals{new float3[totalVerts]};
	
	q.enqueueReadBuffer(verts, CL_TRUE, 0, sizeof(float3) * totalVerts, hVertices.get());
	q.enqueueReadBuffer(normals, CL_TRUE, 0, sizeof(float3) * totalVerts, hNormals.get());
	
	ret.verts.assign(hVertices.get(), hVertices.get() + totalVerts);
	ret.normals.assign(hNormals.get(), hNormals.get() + totalVerts);
	
	return ret;
}

