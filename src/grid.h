#ifndef __KARSTGEN_GRID_H__
#define __KARSTGEN_GRID_H__

#include "math.h"

class Grid
{
protected:
	cl::Context mContext;
	cl::Buffer mValues;
	cl::Buffer mNormals;
	int3 mGridSize;
	float3 mStartPos;
public:
	Grid(
		const int3& gridSize,
		const float3& startPos,
		const cl::Context &context
	);
	
	int3 getGridSize() const { return mGridSize; }
	float3 getStartPos() const { return mStartPos; }
	void setStartPos(const float3& pos) { mStartPos = pos; }
	cl::Buffer getValuesBuffer() const { return mValues; }
	cl::Buffer getNormalsBuffer() const { return mNormals; }
};

#endif
