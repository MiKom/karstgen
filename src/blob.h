#ifndef __KARSTGEN_BLOB_H
#define __KARSTGEN_BLOB_H

#include <vector>

#include "abstractprogram.h"
#include "mathtypes.h"

class Grid;

class Blob : public AbstractProgram
{
protected:
	cl::Kernel mBlobValKernel;
	cl::Kernel mNormalsKernel;
	cl_ulong mConstantBufferSize;
public:
	Blob(
		const cl::Context& context, 
		const std::vector<cl::CommandQueue>& commandQueues
	);
	virtual ~Blob() {}
	
	/**
	  This method adds an array of blobs to the scalar field
	  \param blobs array of blobs to be added. Positions of the blobs are
	  kept in x,y and z components and magnitude (size) of the blob is read
	  from w component.
	  \param nBlobs length of blobs array
	  \param grid grid to which blob values will be added
	  */
	void runBlob(
		const float4* const blobs,
		int nBlobs,
		Grid& grid
	) const;

};

#endif //__KARSTGEN_BLOB_H
