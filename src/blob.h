#ifndef __KARSTGEN_BLOB_H
#define __KARSTGEN_BLOB_H

#include <vector>

#include "abstractprogram.h"
#include "math.h"

class Grid;

/**
  This structure holds definition of single blob in the scene.
  */
struct blob_s {
	float3 pos;		//!< Position of the blob
	float magnitude;	//!< Magnitude, or how big should the blob be
};
typedef struct blob_s blob_t;

class Blob : public AbstractProgram
{
protected:
	cl::Kernel mBlobValKernel;
public:
	Blob(
		const cl::Context& context, 
		const std::vector<cl::CommandQueue>& commandQueues
	);
	virtual ~Blob() {}
	
	/**
	  This method adds an array of blobs to the scalar field
	  \param blobs array of blobs to be added
	  \param nBlobs length of blobs array
	  \param grid grid to which blob values will be added
	  */
	void runBlob(
		const blob_t* const blobs,
		int nBlobs,
		Grid& grid
	) const;

};

#endif //__KARSTGEN_BLOB_H
