struct blob_s {
	cl_float3 pos;
	cl_float magnitude;
};
typedef struct blob_s blob_t;

__kernel
blobValue(
	__global float* values,
	__global float3 normals,
	uint4 gridSize,
	__constant blob_t blobs
	)
{

}

