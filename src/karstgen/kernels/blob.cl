#define EPSILON 0.0001
#define DIV_EPSILON 0.000000001

uint4 calcGridPos(uint i, uint4 gridSize)
{
	uint z = i / (gridSize.x * gridSize.y);
	i -= z * (gridSize.x * gridSize.y);
	uint y = i / gridSize.x;
	i -= y * gridSize.x;
	uint x = i;
	
	return (uint4) (x, y, z, 0);
}

/**
  This kernel adds a set of blobs to the grid.
  Output parameter is values, density function values
  are kept in w component and values of gradient are kept in
  x,y,z components.
  */
__kernel void
blobValue(
	float4 startPoint,
	uint4 gridSize,
	float4 voxelSize,
	__constant float4* blobs,
	int nBlobs,
	__global float4* values,
	int nPoints
	)
{
	uint tid = get_global_id(0);
	uint4 gridPos = calcGridPos(tid, gridSize + (uint4)(1,1,1,0));
	float4 pos;
	pos.x = startPoint.x + gridPos.x * voxelSize.x;
	pos.y = startPoint.y + gridPos.y * voxelSize.y;
	pos.z = startPoint.z + gridPos.z * voxelSize.z;
	pos.w = 1.0f;
	
	float val = values[tid].w;
	float3 norm = values[tid].xyz;
	float4 blob;
	float tmpVal;
	float3 tmpNorm;
	for(int i=0; i<nBlobs; i++) {
		blob = blobs[i];
		blob.w *= blob.w; //WARNING: hack to make blobs roughly the same diameter as w parameter
		tmpVal = blob.w / max((pown(pos.x - blob.x, 2) + pown(pos.y - blob.y, 2) + pown(pos.z - blob.z, 2)), DIV_EPSILON);
		val += tmpVal;
		
		//Calculate gradient
		tmpNorm.x = blob.w / max((pown((pos.x + EPSILON) - blob.x, 2) + pown(pos.y - blob.y, 2) + pown(pos.z - blob.z, 2)), DIV_EPSILON);
		tmpNorm.y = blob.w / max((pown(pos.x - blob.x, 2) + pown((pos.y + EPSILON) - blob.y, 2) + pown(pos.z - blob.z, 2)), DIV_EPSILON);
		tmpNorm.z = blob.w / max((pown(pos.x - blob.x, 2) + pown(pos.y - blob.y, 2) + pown((pos.z + EPSILON) - blob.z, 2)), DIV_EPSILON);
		
		norm += tmpNorm;
	}
	if(tid < nPoints) {
		values[tid] += (float4) (norm.x, norm.y, norm.z, val);
	}
}
