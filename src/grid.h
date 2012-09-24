#ifndef __KARSTGEN_GRID_H__
#define __KARSTGEN_GRID_H__

#include "math.h"

class Grid
{
public:
	enum class Storage {
		DEVICE,
		HOST
	};
protected:
	cl::Context mContext;
	float3 *mValues;
	float3 *mNormals;
	cl::Buffer mValuesBuffer;
	cl::Buffer mNormalsBuffer;
	uint3 mGridDim; /**< Size of the grid, i.e. how many small coboids are
	                  in each dimension. For example, for a grid made of
	                  two small cuboids in x, three in y and four in z pass
	                  (2,3,4) */
	float3 mVoxelSize; /**< Size of the single voxel */
	float3 mStartPos;
	
	Storage mStorage;
public:
	Grid(
		uint3 gridDim,
		float3 voxelSize,
		float3 startPos,
		cl::Context &context
	);
	virtual ~Grid();
	
	uint3 getGridSize() const { return mGridDim; }
	float3 getStartPos() const { return mStartPos; }
	float3 getVoxelSize() const { return mVoxelSize; }
	void setStartPos(const float3& pos) { mStartPos = pos; }
	cl::Buffer getValuesBuffer() const { return mValuesBuffer; }
	cl::Buffer getNormalsBuffer() const { return mNormalsBuffer; }
	Storage getStorage() { return mStorage; }
	
	void copyToDevice();
	void copyToHost();
};

#endif
