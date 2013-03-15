#ifndef __KARSTGEN_GRID_H__
#define __KARSTGEN_GRID_H__

#include "mathtypes.h"

/**
  \brief Storage for 3D grid of values.
  
  This class keeps values of computed density function at lattice vertices of a
  grid of voxels and at epsilon-distant positions along each axis (it's a
  gradient for normal vector calculations). The returned buffer is float4 buffer
  that keeps gradient values in xyz components, and density function value in w
  component.
  
  This class can be fed to MarchingCubes class to run Marching cubes algorithm
  on the lattice.
  
  It can move data between RAM and VRAM.
  */
class Grid
{
public:
	enum class Storage {
		DEVICE, /**< Valid data is currently stored on the device
			     (e.g. VRAM) associated with mContext */
		HOST /**< Valid data is currently stored in host (e.g. RAM) */
	};
protected:
	cl::Context mContext;
	cl::CommandQueue mCommandQueue;
	float4 *mValues;
	cl::Buffer mValuesBuffer;
	uint3 mGridDim; /**< Size of the grid, i.e. how many small voxels are
	                  in each dimension. For example, for a grid made of
	                  two voxels in x, three in y and four in z pass
	                  (2,3,4) */
	float3 mVoxelSize; /**< Size of the single voxel */
	float3 mStartPos;
	
	Storage mStorage;
	
	static unsigned int getFlatDataSize(const uint3& gridDim);
public:

	Grid(
		uint3 gridDim,
		float3 voxelSize,
		float3 startPos,
		cl::Context& context,
		cl::CommandQueue& cq
	);
	//Make grid noncopyable
	Grid(const Grid& other) = delete;
	Grid& operator=(const Grid& other) = delete;
	virtual ~Grid();
	
	uint3 getGridSize() const { return mGridDim; }
	float3 getStartPos() const { return mStartPos; }
	float3 getVoxelSize() const { return mVoxelSize; }
	void setStartPos(const float3& pos) { mStartPos = pos; }
	float4* getValues() const { return mValues; }
	cl::Buffer getValuesBuffer() const { return mValuesBuffer; }
	
	/**
	  \brief Get current storage place.
	  
	  Get information about place where correct data is currently stored for
	  this grid.
	  */
	Storage getStorage() { return mStorage; }
	
	void copyToDevice();
	void copyToHost();
};

#endif
