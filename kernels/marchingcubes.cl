__kernel
void classifyVoxel(
	__global float *gridValues,
	__global uint* voxelVerts,
	__global uint *voxelOccupied,
	uint4 gridSize,
	float4 voxelSize,
	float isoValue,
	uint numVoxels,
	__read_only image2d_t numVertsTex)
{
//TODO: implement
}

__kernel
void compactVoxels(
	__global uint *compactedVoxelArray,
	__global uint *voxelOccupied,
	__global uint *voxelOccupiedScan,
	uint numVoxels
)
{
//TODO: implement
}

__kernel
void generateTriangles(
	__global float4 *pos,
	__global float4 *norm,
	__global float4 *gridValues,
	__global float4 *gridNormals,
	__global uint *compactedVoxelArray,
	__global uint *numVertsScanned,
	uint4 gridSize,
	float4 voxelSize,
	float isoValue,
	uint activeVoxels,
	uint maxVerts,
	__read_only image2d_t numVertsTex,
	__read_only image2d_t triTex
)
{
//TODO: implement
}
