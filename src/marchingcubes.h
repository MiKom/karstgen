#ifndef __KARSTGEN_MARCHINGCUBES_H__
#define __KARSTGEN_MARCHINGCUBES_H__

#include <CL/cl.hpp>
#include "abstractprogram.h"

class vec3;
class Grid;
class MarchingCubes : public AbstractProgram
{
protected:
	cl::Kernel mClassifyVoxelKernel;
	cl::Kernel mCompactVoxelsKernel;
	cl::Kernel mScanKernel;
	cl::Kernel mGenerateTrianglesKernel;
public:
	MarchingCubes(
		cl::Context ctx,
		std::vector<cl::CommandQueue> queues
	);
	virtual ~MarchingCubes() {}

	/**
	  This function computes triangle mesh from scalar field described
	  by grid. Value that will be treated as the frontier of the isosurface
	  is passed in isoValue parameter.
	  
	  \param grid scalar field which describes isosurface
	  \param isoValue value that will be treated as a frontier of the
	  surface
	  \return vector of triangles. Each triangle is a triplet of vec3
	  vectors
	*/
	std::vector<vec3>* compute(const Grid& grid, const float& isoValue);
};

#endif
