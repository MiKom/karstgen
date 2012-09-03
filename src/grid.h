#ifndef __KARSTGEN_GRID_H__
#define __KARSTGEN_GRID_H__

#include "math.h"

class Grid
{
protected:
	cl::Context mContext;
	cl::Buffer mValues;
	cl::Buffer mNormals;
	uint3 mGridSize; /**< Size of the grid, i.e. how many small coboids are
	                   in each dimension. For example, for a grid made of
	                   two small cuboids in x, three in y and four in z pass
	                   (2,3,4) */
	float3 mGridDim; /**< Dimension of the grid, i.e. how much space in each
	                   dimension the whole grid occupies. For cubical grid,
	                   every dimension has to have the same length */
	float3 mStartPos;
public:
	Grid(
		uint3 gridSize,
		float3 gridDim,
		float3 startPos,
		cl::Context &context
	);
	
	uint3 getGridSize() const { return mGridSize; }
	float3 getStartPos() const { return mStartPos; }
	float3 getGridDim() const { return mGridDim; }
	void setStartPos(const float3& pos) { mStartPos = pos; }
	cl::Buffer getValuesBuffer() const { return mValues; }
	cl::Buffer getNormalsBuffer() const { return mNormals; }
};

#endif
