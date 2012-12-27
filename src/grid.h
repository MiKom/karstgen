#ifndef __KARSTGEN_GRID_H__
#define __KARSTGEN_GRID_H__

#include "math.h"

/**
  This class keeps values of computed density function and its normalsalong the
  grid of voxels. The returned buffer is float4 buffer that keeps normal in xyz
  components, and density function value in w component. It can move data
  between RAM and VRAM.
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
	uint3 mGridDim; /**< Size of the grid, i.e. how many small cuboids are
	                  in each dimension. For example, for a grid made of
	                  two small cuboids in x, three in y and four in z pass
	                  (2,3,4) */
	float3 mVoxelSize; /**< Size of the single voxel */
	float3 mStartPos;
	
	Storage mStorage;
	
	/**
	  This function calculates the number of data point on the grid that
	  will be needed to keep the grid data. Basically, for x*y*z sized grid
	  (in voxels in each dimension) you need (x+1) * (y+1) * (z+1) data
	  points. This is a utility function to compute that.
	  */
	static unsigned int getFlatDataSize(const uint3& gridDim);
public:
	/**
	  \param context OpenCL context within which this grid will operate
	  \param cq OpenCL command queue (in the same context as context 
	  parameter) that will be used to move data back and forth between the
	  devices.
	  */
	Grid(
		uint3 gridDim,
		float3 voxelSize,
		float3 startPos,
		cl::Context &context,
		cl::CommandQueue &cq
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
	  Get information about place where correct data is currently stored for
	  this grid.
	  */
	Storage getStorage() { return mStorage; }
	
	void copyToDevice();
	void copyToHost();
};

#endif
