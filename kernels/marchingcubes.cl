#define NTHREADS 32

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

void getCubeValues(
	uint4 voxelPos,
	__global float4 *gridValues,
	uint4 dataGridSize,
	float4 *values)
{
	int vertexIndex;
	vertexIndex = calcFlatPos(voxelPos, dataGridSize);
	values[0] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(1,0,0,0), dataGridSize));
	values[1] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(1,1,0,0), dataGridSize));
	values[2] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(0,1,0,0), dataGridSize));
	values[3] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(0,0,1,0), dataGridSize));
	values[4] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(1,0,1,0), dataGridSize));
	values[5] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(1,1,1,0), dataGridSize));
	values[6] = gridValues[vertexIndex];

	vertexIndex = (calcFlatPos(voxelPos + (uint4)(0,1,1,0), dataGridSize));
	values[7] = gridValues[vertexIndex];
}

__kernel
void classifyVoxel(
	__global float4 *gridValues,
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
	
	float4 cubeValues[8];
	getCubeValues(voxelGridPos, gridValues, dataGridSize, cubeValues);

	//Loop unrolled for better performance
	int cubeIndex;
	cubeIndex =  (cubeValues[0].w < isoValue);
	cubeIndex += (cubeValues[1].w < isoValue) << 1;
	cubeIndex += (cubeValues[2].w < isoValue) << 2;
	cubeIndex += (cubeValues[3].w < isoValue) << 3;
	cubeIndex += (cubeValues[4].w < isoValue) << 4;
	cubeIndex += (cubeValues[5].w < isoValue) << 5;
	cubeIndex += (cubeValues[6].w < isoValue) << 6;
	cubeIndex += (cubeValues[7].w < isoValue) << 7;
	
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

void vertexInterp(
	float isoLevel,
	float4 p1,
	float4 p2,
	float4 f1,
	float4 f2,
	float4 *pos,
	float4 *norm)
{
	float t = (isoLevel - f1.w) / (f2.w - f1.w);
	*pos = mix(p1, p2, t);
	*norm = mix(f1, f2, t);
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
	uint i = get_global_id(0);
	uint tid = get_local_id(0);
	
	if(i > activeVoxels - 1) {
		i = activeVoxels - 1;
	}
	
	uint voxel = compactedVoxelArray[i];
	
	uint4 gridPos = calcGridPos(voxel, gridSize);
	
	float4 p;
	p.x = startPoint.x + gridPos.x * voxelSize.x;
	p.y = startPoint.y + gridPos.y * voxelSize.y;
	p.z = startPoint.z + gridPos.z * voxelSize.z;
	p.w = 1.0f;
	
	float4 verts[8];
	verts[0] = p;
	verts[1] = p + (float4)(voxelSize.x, 0, 0, 0);
	verts[2] = p + (float4)(voxelSize.x, voxelSize.y, 0, 0);
	verts[3] = p + (float4)(0, voxelSize.y, 0, 0);
	verts[4] = p + (float4)(0, 0, voxelSize.z, 0);
	verts[5] = p + (float4)(voxelSize.x, 0, voxelSize.z, 0);
	verts[6] = p + (float4)(voxelSize.x, voxelSize.y, voxelSize.z, 0);
	verts[7] = p + (float4)(0, voxelSize.y, voxelSize.z, 0);
	
	__local float4 vertList[12*NTHREADS];
	__local float4 normList[12*NTHREADS];
	
	//TODO: implement
}
