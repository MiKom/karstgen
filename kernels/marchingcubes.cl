uint4 calcGridPos(uint i, uint4 gridSize)
{
	uint z = i / (gridSize.x * gridSize.y)
	i -= z * (gridSize.x * gridSize.y);
	uint y = i / gridSize.x
	i -= y * gridSize.x;
	uint x = i
	
	return (uint4) (x, y, z, 0);
}

__kernel
void classifyVoxel(
	__global float *gridValues,
	__global uint *voxelVerts,
	__global uint *voxelOccupied,
	uint4 gridSize,
	float4 voxelSize,
	float isoValue,
	uint numVoxels,
	__read_only image2d_t numVertsTex)
{
	
}

__kernel
void compactVoxels(
	__global uint *compactedVoxelArray,
	__global uint *voxelOccupied,
	__global uint *voxelOccupiedScan,
	uint numVoxels
)
{
	uint i = get_global_id(0);
	if(voxelOccupied[i] && (i < numVoxels)) {
		compactedVoxelArray[voxelOccupiedScan[i]] = i;
	}
}

__kernel
void generateTriangles(
	__global float4 *pos,
	__global float4 *norm,
	__global float4 *gridValues,
	__global uint *compactedVoxelArray,
	__global uint *voxelVertsScanned,
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
