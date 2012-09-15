#ifndef __KARSTGEN_MARCHINGCUBES_H__
#define __KARSTGEN_MARCHINGCUBES_H__

#include "abstractprogram.h"
#include "math.h"

class Grid;
class Scan;

typedef struct {
	std::vector<float3> *verts;
	std::vector<float3> *normals;
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
	
	void launchClassifyVoxel(
		Grid* grid,
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
	
	void launchgenerateTriangles(
		cl::Buffer pos,
		cl::Buffer norm,
		cl::Buffer compVoxelArray,
		cl::Buffer numVertsScanned, 
		float isoValue,
		unsigned int activeVoxels,
		unsigned int maxVerts,
		Grid* grid
	);
public:
	/**
	  \param scan pointer to object storing initialized scan operator
	*/
	MarchingCubes(
		const cl::Context ctx,
		const std::vector<cl::CommandQueue> &queues,
		Scan* scan
	);
	virtual ~MarchingCubes() {}

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
	MCMesh compute(const Grid& grid, float isoValue) const;
};

#endif
