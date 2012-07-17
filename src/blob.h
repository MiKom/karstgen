#ifndef __KARSTGEN_BLOB_H
#define __KARSTGEN_BLOB_H

#include "abstractprogram.h"

class Grid;

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
	
	/**
	  This method adds an array of blobs to the scalar field
	  \param blobs array of blobs to be added
	  \param nBlobs length of blobs array
	  */
	void runBlob(
		blob_t* blobs,
		int nBlobs,
		Grid& grid
	);

};

#endif //__KARSTGEN_BLOB_H
