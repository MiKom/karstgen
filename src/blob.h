#ifndef __KARSTGEN_BLOB_H
#define __KARSTGEN_BLOB_H

#include "abstractprogram.h"

/**
  This structure holds definition of single blob in the scene.
  */
struct blob_s {
	cl_float4 pos;		//!< Position of the blob
	cl_float magnitude;	//!< Magnitude, or how big should the blob be
};
typedef struct blob_s blob_t;

class Blob : public AbstractProgram
{
protected:
	cl::Kernel mBlobValKernel;
public:
	Blob(cl::Context& context);
	
	void runBlob(
		blob_t* blobs,
		int nBlobs,
		cl::Buffer& val,
		cl::Buffer& norm,
		cl_int3 gridSize,
		cl_float3 startPos
	);

};

void initBlob(cl::Context& context);


#endif //__KARSTGEN_BLOB_H
