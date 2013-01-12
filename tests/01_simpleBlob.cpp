#include "config.h"
#include "context.h"
#include "grid.h"
#include "blob.h"
#include "marchingcubes.h"
#include "util.h"

#include <iostream>
#include <CL/cl.hpp>

using namespace std;

int main()
{
	try {
		Context ctx;
		cl::CommandQueue q = ctx.getQueues()[0];
		Grid grid(
			uint3(64,64,64),
			float3(5.0f/64.0f, 5.0f / 64.0f, 5.0f / 64.0f),
			float3(-5.0f, -5.0f, -5.0f),
			ctx.getClContext(),
			ctx.getQueues()[0]
		);
		
		float3 blobs[] = { {0.0f, 0.0f, 0.0f, 3.0f} };
		
		Blob* blobProgram = ctx.getBlobProgram();
	
		blobProgram->runBlob(blobs, 1, grid);
		
		MarchingCubes* mc = ctx.getMcProgram();
		mc->compute(grid, 1.0f);
	} catch ( cl::Error &e ) {
		cerr
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
		return 1;
	}
	
	return 0;
}
