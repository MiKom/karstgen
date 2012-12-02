typedef unsigned int uint;
sampler_t tableSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

uint4 calcGridPos(uint i, uint4 gridSize)
{
	uint z = i / (gridSize.x * gridSize.y);
	i -= z * (gridSize.x * gridSize.y);
	uint y = i / gridSize.x;
	i -= y * gridSize.x;
	uint x = i;
	
	return (uint4) (x, y, z, 0);
}

uint calcFlatPos(uint4 gridPos, uint4 gridSize)
{
	uint position = 0;
	position += gridPos.z * gridSize.x * gridSize.y;
	position += gridPos.y * gridSize.x;
	position += gridPos.x;
	return position;
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
	uint4 dataGridSize = gridSize + (uint4) (1,1,1,0);
	
	uint i = get_global_id(0);
	uint4 voxelGridPos = calcGridPos(i, gridSize);
	int cubeIndex;

	int vertexIndex;
	vertexIndex = calcFlatPos(voxelGridPos, dataGridSize);
	cubeIndex = gridValues[vertexIndex] < isoValue;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(1,0,0,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 1;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(1,1,0,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 2;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(0,1,0,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 3;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(0,0,1,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 4;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(1,0,1,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 5;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(1,1,1,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 6;

	vertexIndex = (calcFlatPos(voxelGridPos + (uint4)(0,1,1,0), dataGridSize));
	cubeIndex += (gridValues[vertexIndex] < isoValue) << 7;
	
	uint numVerts = read_imageui(numVertsTex, tableSampler, (int2)(cubeIndex, 0)).x;
	if (i < numVoxels) {
		voxelVerts[i] = numVerts;
		voxelOccupied[i] = (numVerts > 0);
	}
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
	float4 startPoint,
	float isoValue,
	uint activeVoxels,
	uint maxVerts,
	__read_only image2d_t numVertsTex,
	__read_only image2d_t triTex
)
{
//TODO: implement
}
