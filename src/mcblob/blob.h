#ifndef __MCBLOB_BLOB_H
#define __MCBLOB_BLOB_H

#include <vector>

#include "abstractprogram.h"
#include "common/mathtypes.h"

class Grid;

class Blob : public AbstractProgram
{
protected:
	cl::Kernel mBlobValKernel;
	cl_ulong mConstantBufferSize;
public:
	Blob(
		const cl::Context& context, 
		const std::vector<cl::CommandQueue>& commandQueues
	);
	virtual ~Blob() {}
	
	void runBlob(
		const float4* const blobs,
		int nBlobs,
		Grid& grid
	);

};

#endif //__MCBLOB_BLOB_H
