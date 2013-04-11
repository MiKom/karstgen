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

int getCubeIndex(float4 *cubeValues, float isoValue)
{
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
	return cubeIndex;
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

	int cubeIndex = getCubeIndex(cubeValues, isoValue);
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
	__local float4 *pos,
	__local float4 *norm)
{
	float t = (isoLevel - f1.w) / (f2.w - f1.w);

	//Calculating normals from gradient
	float4 norm1 = (float4) (f1.x - f1.w, f1.y - f1.w, f1.z - f1.w, 0.0f);
	float4 norm2 = (float4) (f2.x - f2.w, f2.y - f2.w, f2.z - f2.w, 0.0f);

	*pos = mix(p1, p2, t);
	*norm = mix(norm1, norm1, t);
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
	
	uint4 dataGridSize = gridSize + (uint4) (1,1,1,0);
	float4 cubeValues[8];
	getCubeValues(gridPos, gridValues, dataGridSize, cubeValues);
	
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
	
	vertexInterp(isoValue, verts[0], verts[1], cubeValues[0], cubeValues[1], &vertList[tid], &normList[tid]);
	vertexInterp(isoValue, verts[1], verts[2], cubeValues[1], cubeValues[2], &vertList[NTHREADS+tid], &normList[NTHREADS+tid]);
	vertexInterp(isoValue, verts[2], verts[3], cubeValues[2], cubeValues[3], &vertList[NTHREADS*2+tid], &normList[NTHREADS*2+tid]);
	vertexInterp(isoValue, verts[3], verts[0], cubeValues[3], cubeValues[0], &vertList[NTHREADS*3+tid], &normList[NTHREADS*3+tid]);
	vertexInterp(isoValue, verts[4], verts[5], cubeValues[4], cubeValues[5], &vertList[NTHREADS*4+tid], &normList[NTHREADS*4+tid]);
	vertexInterp(isoValue, verts[5], verts[6], cubeValues[5], cubeValues[6], &vertList[NTHREADS*5+tid], &normList[NTHREADS*5+tid]);
	vertexInterp(isoValue, verts[6], verts[7], cubeValues[6], cubeValues[7], &vertList[NTHREADS*6+tid], &normList[NTHREADS*6+tid]);
	vertexInterp(isoValue, verts[7], verts[4], cubeValues[7], cubeValues[4], &vertList[NTHREADS*7+tid], &normList[NTHREADS*7+tid]);
	vertexInterp(isoValue, verts[0], verts[4], cubeValues[0], cubeValues[4], &vertList[NTHREADS*8+tid], &normList[NTHREADS*8+tid]);
	vertexInterp(isoValue, verts[1], verts[5], cubeValues[1], cubeValues[5], &vertList[NTHREADS*9+tid], &normList[NTHREADS*9+tid]);
	vertexInterp(isoValue, verts[2], verts[6], cubeValues[2], cubeValues[6], &vertList[NTHREADS*10+tid], &normList[NTHREADS*10+tid]);
	vertexInterp(isoValue, verts[3], verts[7], cubeValues[3], cubeValues[7], &vertList[NTHREADS*11+tid], &normList[NTHREADS*11+tid]);
	barrier(CLK_LOCAL_MEM_FENCE);
	
	int cubeIndex = getCubeIndex(cubeValues, isoValue);
	uint numVerts = read_imageui(numVertsTex, tableSampler, (int2)(cubeIndex, 0)).x;
	
	for(int i=0; i<numVerts; i+=3) {
		uint index = voxelVertsScanned[voxel] + i;
		float4 positions[3];
		float4 normals[3];
		uint edge;
		
		edge = read_imageui(triTex, tableSampler, (int2)(i, cubeIndex)).x;
		positions[0] = vertList[(edge*NTHREADS) + tid];
		normals[0] = normList[(edge*NTHREADS) + tid];
		
		edge = read_imageui(triTex, tableSampler, (int2)(i+1, cubeIndex)).x;
		positions[1] = vertList[(edge*NTHREADS) + tid];
		normals[1] = normList[(edge*NTHREADS) + tid];
		
		edge = read_imageui(triTex, tableSampler, (int2)(i+2, cubeIndex)).x;
		positions[2] = vertList[(edge*NTHREADS) + tid];
		normals[2] = normList[(edge*NTHREADS) + tid];
		if(index <= (maxVerts - 3)) {
			pos[index] = positions[0];
			norm[index] = normalize(normals[0]);

			pos[index+1] = positions[1];
			norm[index+1] = normalize(normals[1]);

			pos[index+2] = positions[2];
			norm[index+2] = normalize(normals[2]);
		}
	}
}
