#ifndef __MCBLOB_MARCHINGCUBES_H__
#define __MCBLOB_MARCHINGCUBES_H__

#include "abstractprogram.h"
#include "common/mathtypes.h"

class Grid;
class Scan;

typedef struct {
	std::vector<float3> verts;
	std::vector<float3> normals;
} MCMesh;

class MarchingCubes : public AbstractProgram
{
protected:
	
	//kernels
	cl::Kernel mClassifyVoxelKernel;
	cl::Kernel mCompactVoxelsKernel;
	cl::Kernel mGenerateTrianglesKernel;
	
	//textures with tables
	cl::Image2D mTriangleTable;
	cl::Image2D mNumVertsTable;
	
	Scan* mScanOp;
	
public:
	void launchClassifyVoxel(
		const Grid& grid,
		cl::Buffer voxelVerts,
		cl::Buffer voxelOccupied,
		float isoValue
	);
	
	void launchCompactVoxels(
		cl::Buffer compVoxelArray,
		cl::Buffer voxelOccupied,
		cl::Buffer voxelOccupiedScan,
		unsigned int numVoxels
	);
	
	void launchGenerateTriangles(
		cl::Buffer pos,
		cl::Buffer norm,
		cl::Buffer compVoxelArray,
		cl::Buffer numVertsScanned, 
		float isoValue,
		unsigned int activeVoxels,
		unsigned int maxVerts,
		const Grid& grid
	);
	MarchingCubes(
		const cl::Context ctx,
		const std::vector<cl::CommandQueue> &queues,
		Scan* scan
	);
	virtual ~MarchingCubes() {}

	MCMesh compute(Grid &grid, float isoValue);
};

#endif
