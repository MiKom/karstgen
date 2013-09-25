#define EPSILON 0.0001

#define BLOBINESS 1.0f

uint4 calcGridPos(uint i, uint4 gridSize)
{
	uint z = i / (gridSize.x * gridSize.y);
	i -= z * (gridSize.x * gridSize.y);
	uint y = i / gridSize.x;
	i -= y * gridSize.x;
	uint x = i;
	
	return (uint4) (x, y, z, 0);
}

float
singleBlobVal(float4 blob, float4 pos)
{
	float4 dist = (float4)(blob.xyz - pos.xyz, 0.0f);
	
	return native_exp(
		-BLOBINESS * native_recip(blob.w*blob.w) * dot(dist, dist) + BLOBINESS
	);
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
		blob.w /= 2.0f; //HACK: we have diameter in parameter, but equations in form below treat .w as radius
		
		tmpVal = singleBlobVal(blob, pos);
		val += tmpVal;
		
		//Calculate gradient
		tmpNorm.x = singleBlobVal(blob, pos + (float4)(EPSILON, 0.0f, 0.0f, 0.0f));
		tmpNorm.y = singleBlobVal(blob, pos + (float4)(0.0f, EPSILON, 0.0f, 0.0f));
		tmpNorm.z = singleBlobVal(blob, pos + (float4)(0.0f, 0.0f, EPSILON, 0.0f));
		
		norm += tmpNorm;
	}
	if(tid < nPoints) {
		values[tid] += (float4) (norm.x, norm.y, norm.z, val);
	}
}
