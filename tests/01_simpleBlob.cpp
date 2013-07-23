#include "config.h"
#include "context.h"
#include "grid.h"
#include "blob.h"
#include "marchingcubes.h"
#include "exporters.h"
#include "util.h"

#include <iostream>
#include <CL/cl.hpp>
#include <avr/avr++.h>

using namespace std;
using namespace AVR;

int main()
{
	try {
		Context ctx{true};
		cl::CommandQueue q = ctx.getQueues()[0];
		Grid grid(
			uint3(64,64,64),
			float3(15.0f/64.0f, 15.0f / 64.0f, 15.0f / 64.0f),
			float3(-2.5f, -2.5f, -2.5f),
			ctx.getClContext(),
			ctx.getQueues()[0]
		);
		
		float3 blobs[] = { {0.0f, 0.0f, 0.0f, 1.94f}, {0.0f, 0.5f, 2.0f, 2.0f} };
		
		Blob* blobProgram = ctx.getBlobProgram();
	
		blobProgram->runBlob(blobs, sizeof(blobs) / sizeof(blobs[0]), grid);
		
		
		MarchingCubes* mc = ctx.getMcProgram();
		MCMesh mcmesh = mc->compute(grid, 1.0f);
		
		vector<MCMesh> meshes {mcmesh};
		export_avr(meshes, "out.avr");
		export_wavefront_obj(meshes, "out.obj");
	
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
