struct blob_s {
	float3 pos;
	float magnitude;
};
typedef struct blob_s blob_t;

__kernel void
blobValue(
	__global float* values,
	__global float3* normals,
	float4 startPoint,
	uint4 gridSize,
	__constant blob_t* blobs
	)
{

}

